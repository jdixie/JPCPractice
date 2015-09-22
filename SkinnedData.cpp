#include "SkinnedData.h"

float AnimationClip::GetClipStartTime()const
{
	// Find smallest start time over all bones in this clip.
	float t = FLT_MAX;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		if (t > BoneAnimations[i].GetStartTime())
			t = BoneAnimations[i].GetStartTime();
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	// Find largest end time over all bones in this clip.
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		if (t < BoneAnimations[i].GetStartTime())
			t = BoneAnimations[i].GetStartTime();
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<XMMATRIX>& boneTransforms)const
{
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		BoneAnimations[i].Interpolate(t, boneTransforms[i]);
	}
}

float SkinnedData::GetClipStartTime(const std::string& clipName)const
{
	auto clip = animations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName)const
{
	auto clip = animations.find(clipName);
	return clip->second.GetClipEndTime();
}

UINT SkinnedData::BoneCount()const
{
	return boneHierarchy.size();
}

void SkinnedData::Set(std::vector<int> &bh, std::vector<XMMATRIX> &bo, std::map<std::string, AnimationClip> &a)
{
	boneHierarchy = bh;
	boneOffsets = bo;
	animations = a;
}

void SkinnedData::GetFinalTransforms(const std::string& clipName, float timePos, std::vector<XMMATRIX>& finalTransforms)const
{
	UINT numBones = boneOffsets.size();

	std::vector<XMMATRIX> toParentTransforms(numBones);

	// Interpolate all the bones of this clip at the given time instance.
	auto clip = animations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	//
	// Traverse the hierarchy and transform all the bones to the root space.
	//

	std::vector<XMMATRIX> toRootTransforms(numBones);

	// The root bone has index 0.  The root bone has no parent, so its toRootTransform
	// is just its local bone transform.
	toRootTransforms[0] = toParentTransforms[0];

	// Now find the toRootTransform of the children.
	for (UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = toParentTransforms[i];

		int parentIndex = boneHierarchy[i];
		XMMATRIX parentToRoot = toRootTransforms[parentIndex];

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		toRootTransforms[i] = toRoot;
	}

	// Premultiply by the bone offset transform to get the final transform.
	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = boneOffsets[i];
		XMMATRIX toRoot = toRootTransforms[i];
		finalTransforms[i] = XMMatrixMultiply(offset, toRoot);
	}
}