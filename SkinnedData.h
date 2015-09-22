#ifndef SKINNED_DATA_H
#define SKINNED_DATA_H

#include <DirectXMath.h>
#include <vector>
#include <map>
#include <float.h>
#include <cmath>
#include "constants.h"

using namespace DirectX;

struct Keyframe
{
	Keyframe()
	: TimePos(0.0f),
	Translation(0.0f, 0.0f, 0.0f),
	Scale(1.0f, 1.0f, 1.0f),
	RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
	{}
	~Keyframe()
	{}

	float TimePos;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT3 Scale;
	DirectX::XMFLOAT4 RotationQuat;
};

struct BoneAnimation
{
	std::vector<Keyframe> Keyframes;
	float GetStartTime() const{ return Keyframes.front().TimePos; }
	float GetEndTime() const{ return Keyframes.back().TimePos; }
	void Interpolate(float t, DirectX::XMMATRIX &M) const
	{
		{
			if (t <= Keyframes.front().TimePos)
			{
				DirectX::XMVECTOR S = XMLoadFloat3(&Keyframes.front().Scale);
				DirectX::XMVECTOR P = XMLoadFloat3(&Keyframes.front().Translation);
				DirectX::XMVECTOR Q = XMLoadFloat4(&Keyframes.front().RotationQuat);

				DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

				M = DirectX::XMMatrixAffineTransformation(S, zero, Q, P);
			}
			else if (t >= Keyframes.back().TimePos)
			{
				DirectX::XMVECTOR S = XMLoadFloat3(&Keyframes.back().Scale);
				DirectX::XMVECTOR P = XMLoadFloat3(&Keyframes.back().Translation);
				DirectX::XMVECTOR Q = XMLoadFloat4(&Keyframes.back().RotationQuat);

				DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				M = DirectX::XMMatrixAffineTransformation(S, zero, Q, P);
			}
			else
			{
				for (UINT i = 0; i < Keyframes.size() - 1; ++i)
				{
					if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos)
					{
						float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

						DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&Keyframes[i].Scale);
						DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&Keyframes[i + 1].Scale);

						DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&Keyframes[i].Translation);
						DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&Keyframes[i + 1].Translation);

						DirectX::XMVECTOR q0 = DirectX::XMLoadFloat4(&Keyframes[i].RotationQuat);
						DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&Keyframes[i + 1].RotationQuat);

						DirectX::XMVECTOR S = DirectX::XMVectorLerp(s0, s1, lerpPercent);
						DirectX::XMVECTOR P = DirectX::XMVectorLerp(p0, p1, lerpPercent);
						DirectX::XMVECTOR Q = DirectX::XMQuaternionSlerp(q0, q1, lerpPercent);

						DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
						M = DirectX::XMMatrixAffineTransformation(S, zero, Q, P);

						break;
					}
				}
			}
		}
	}
};

struct AnimationClip
{
	float GetClipStartTime() const;
	float GetClipEndTime() const;
	void Interpolate(float t, std::vector<DirectX::XMMATRIX> &boneTransforms) const;
	std::vector<BoneAnimation> BoneAnimations;
};

class SkinnedData
{
public:

	UINT BoneCount()const;

	float GetClipStartTime(const std::string &clipName)const;
	float GetClipEndTime(const std::string &clipName)const;

	void Set(std::vector<int> &bh, std::vector<DirectX::XMMATRIX> &bo, std::map<std::string, AnimationClip> &a);

	//create a cache for this result for static animtaion positions
	void GetFinalTransforms(const std::string &clipName, float timePos, std::vector<DirectX::XMMATRIX> &finalTransforms)const;

private:
	// Gives parentIndex of ith bone.
	std::vector<int> boneHierarchy;

	std::vector<DirectX::XMMATRIX> boneOffsets;

	std::map<std::string, AnimationClip> animations;
};

#endif