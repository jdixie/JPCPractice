#include "Image.h"

Image::Image()
{
	imageInfo.x = 0.0;
	imageInfo.y = 0.0;
	imageInfo.z = 0.0;
	imageInfo.rx = 0.0;
	imageInfo.ry = 0.0;
	imageInfo.rz = 0.0;
	width = 0;
	height = 0;
	imageInfo.scale = 1.0;
	imageInfo.blendFactors[0] = 0.0;
	imageInfo.blendFactors[1] = 0.0;
	imageInfo.blendFactors[2] = 0.0;
	imageInfo.blendFactors[3] = 0.0;
	imageInfo.color[0] = 0.5;
	imageInfo.color[1] = 0.5;
	imageInfo.color[2] = 0.5;
	imageInfo.color[3] = 0.5;
	visible = false;
	looping = true;
	flipHorizontal = false;
	flipVertical = false;
}

Image::~Image()
{
	safeRelease(imageInfo.vertexBuffer);
	safeRelease(imageInfo.indexBuffer);
	for (int i = imageInfo.mesh->textureMapSRV.size() - 1; i >= 0; i--)
		if (imageInfo.mesh->textureMapSRV.at(i) != NULL)
			safeRelease(imageInfo.mesh->textureMapSRV.at(i));
	delete imageInfo.mesh;
}

//initializer
bool Image::initialize(Graphics *g, int w, int h, float sc)
{
	graphics = g;
	width = w;
	height = h;
	imageInfo.scale = sc;

	return true;
}

void Image::setColorVertices()
{
	imageInfo.totalVertices = imageInfo.mesh->colorVertices.size();
	imageInfo.totalIndices = imageInfo.mesh->indices.size();

	D3D11_BUFFER_DESC vbdt;
	vbdt.Usage = D3D11_USAGE_IMMUTABLE;
	vbdt.ByteWidth = sizeof(ColorVertex)* imageInfo.totalVertices;
	vbdt.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbdt.CPUAccessFlags = 0;
	vbdt.MiscFlags = 0;
	vbdt.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srdt;
	srdt.pSysMem = &imageInfo.mesh->colorVertices[0];
	graphics->getDevice()->CreateBuffer(&vbdt, &srdt, &imageInfo.vertexBuffer);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* imageInfo.totalIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srdi;
	srdi.pSysMem = &imageInfo.mesh->indices[0];

	graphics->getDevice()->CreateBuffer(&ibd, &srdi, &imageInfo.indexBuffer);
}

void Image::setVertices()
{
	imageInfo.totalVertices = imageInfo.mesh->vertices.size();
	imageInfo.totalIndices = imageInfo.mesh->indices.size();

	D3D11_BUFFER_DESC vbdt;
	vbdt.Usage = D3D11_USAGE_IMMUTABLE;
	vbdt.ByteWidth = sizeof(Vertex)* imageInfo.totalVertices;
	vbdt.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbdt.CPUAccessFlags = 0;
	vbdt.MiscFlags = 0;
	vbdt.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srdt;
	srdt.pSysMem = &imageInfo.mesh->vertices[0];
	graphics->getDevice()->CreateBuffer(&vbdt, &srdt, &imageInfo.vertexBuffer);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* imageInfo.totalIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srdi;
	srdi.pSysMem = &imageInfo.mesh->indices[0];

	graphics->getDevice()->CreateBuffer(&ibd, &srdi, &imageInfo.indexBuffer);
}

void Image::draw()
{
	if (imageInfo.mesh->textureMapSRV.at(0) != NULL)
		graphics->drawTextured(&imageInfo);
	else
		graphics->drawColored(&imageInfo);
}

void Image::update(float frameTime)
{

}