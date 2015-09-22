#include "mesh.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

MeshHandler::MeshHandler()
{

}

MeshHandler::~MeshHandler()
{
	meshTable.clear();
}

void MeshHandler::initialize(Graphics *g)
{
	graphics = g;
}

//load NPS binary file, in order:
//vertices, indices, materials, bones, animations
HRESULT MeshHandler::loadMesh(ImageInfo *image, const wchar_t *filename)
{
	fMinX = 0.0f;
	fMaxX = 0.0f;
	fMinY = 0.0f;
	fMaxY = 0.0f;
	fMinZ = 0.0f;
	fMaxZ = 0.0f;

	WCHAR wstrOldDir[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, wstrOldDir);

	std::wostringstream toConvert;
	toConvert << filename;
	const std::wstring fileName(toConvert.str());
	meshIterator = meshTable.find(fileName);

	if (meshIterator == meshTable.end()) //if we haven't loaded this mesh yet, load it now
	{
		WCHAR strMaterialFilename[MAX_PATH] = { 0 };
		WCHAR wstr[MAX_PATH];
		char str[MAX_PATH];
		HRESULT hr;

		MeshData *tempMesh = new MeshData;

		//initMaterial(pMaterial);

		DWORD dwCurSubset = 0;

		// File input
		FILE *file;
		_wfopen_s(&file, filename, L"rb");

		fseek(file, 0, SEEK_END);
		long filesize = ftell(file);
		rewind(file);

		if (file == NULL)
		{
			throw(GameError(gameErrorNS::FATAL_ERROR, L"Error: mesh data missing."));
		}

		for (;;)
		{
			unsigned short chunk_id;
			unsigned int chunk_length;
			unsigned char byteData;

			//read # of vertices, indices, materials, bones, animations
			fread(&image->totalMaterials, sizeof(unsigned int), 1, file);
			fread(&image->totalVertices, sizeof(unsigned int), 1, file);
			fread(&image->totalIndices, sizeof(unsigned int), 1, file);			
			fread(&image->totalBones, sizeof(unsigned int), 1, file);
			fread(&image->totalAnimations, sizeof(unsigned int), 1, file);

			//read materials
			for (int i = 0; i < image->totalMaterials; i++)
			{
				MaterialProperties *tempMat = new MaterialProperties();

				fread(&tempMat->Mat.Emissive.x, sizeof(float), 1, file);
				fread(&tempMat->Mat.Emissive.y, sizeof(float), 1, file);
				fread(&tempMat->Mat.Emissive.z, sizeof(float), 1, file);
				fread(&tempMat->Mat.Emissive.w, sizeof(float), 1, file);
				fread(&tempMat->Mat.Ambient.x, sizeof(float), 1, file);
				fread(&tempMat->Mat.Ambient.y, sizeof(float), 1, file);
				fread(&tempMat->Mat.Ambient.z, sizeof(float), 1, file);
				fread(&tempMat->Mat.Ambient.w, sizeof(float), 1, file);
				fread(&tempMat->Mat.Diffuse.x, sizeof(float), 1, file);
				fread(&tempMat->Mat.Diffuse.y, sizeof(float), 1, file);
				fread(&tempMat->Mat.Diffuse.z, sizeof(float), 1, file);
				fread(&tempMat->Mat.Diffuse.w, sizeof(float), 1, file);
				fread(&tempMat->Mat.Specular.x, sizeof(float), 1, file);
				fread(&tempMat->Mat.Specular.y, sizeof(float), 1, file);
				fread(&tempMat->Mat.Specular.z, sizeof(float), 1, file);
				fread(&tempMat->Mat.Specular.w, sizeof(float), 1, file);
				fread(&tempMat->Mat.SpecularPower, sizeof(float), 1, file);
				tempMat->Mat.UseTexture = 1;

				//add it
				image->mesh->materials.push_back(*tempMat);
			}

			//read vertices
			//position(3), normal(3), color(4), tex-coords(2), blend info(index, weight)(4)
			for (int i = 0; i < image->totalVertices; i++)
			{
				Vertex *tempVertex = new Vertex;
				
				fread(&tempVertex->position.x, sizeof(float), 1, file);
				fread(&tempVertex->position.y, sizeof(float), 1, file);
				fread(&tempVertex->position.z, sizeof(float), 1, file);
				fread(&tempVertex->normal.x, sizeof(float), 1, file);
				fread(&tempVertex->normal.y, sizeof(float), 1, file);
				fread(&tempVertex->normal.z, sizeof(float), 1, file);
				fread(&tempVertex->color.x, sizeof(float), 1, file);
				fread(&tempVertex->color.y, sizeof(float), 1, file);
				fread(&tempVertex->color.z, sizeof(float), 1, file);
				fread(&tempVertex->color.w, sizeof(float), 1, file);
				fread(&tempVertex->tex.x, sizeof(float), 1, file);
				fread(&tempVertex->tex.y, sizeof(float), 1, file);

				for (int j = 0; j < 4; j++)
				{
					fread(&tempVertex->blendWeights[j].x, sizeof(float), 1, file);
					fread(&tempVertex->blendWeights[j].y, sizeof(float), 1, file);
				}


				//add it
				image->mesh->vertices.push_back(*tempVertex);
			}

			//read indices
			for (int i = 0; i < image->totalIndices; i++)
			{
				unsigned int tempIndex;

				fread(&tempIndex, sizeof(unsigned int), 1, file);

				//add it
				image->mesh->indices.push_back(tempIndex);
			}

			//read bones
			for (int i = 0; i < image->totalBones; i++)
			{

			}

			//read animations
			for (int i = 0; i < image->totalAnimations; i++)
			{

			}
		}

	}
}

HRESULT MeshHandler::LoadMaterialsFromMTL(const wchar_t *file, MeshData *tempMesh)
{
	/*HRESULT hr;

	// concatenate the mesh\ to the beginning of the filename
	wchar_t filename[MAX_PATH] = { 'm', 'e', 's', 'h', '\\' };
	wcscat_s(filename, file);

	// File input
	WCHAR strCommand[256] = { 0 };
	std::wifstream inFile(filename);
	if (!inFile)
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, L"Error in archaic mesh loading 3."));
	}

	Material *pMaterial = NULL;
	int index;

	for (;;)
	{
		inFile >> strCommand;
		if (!inFile)
			break;

		if (0 == wcscmp(strCommand, L"newmtl"))
		{
			// Switching active materials
			WCHAR strName[MAX_PATH] = { 0 };
			inFile >> strName;

			pMaterial = NULL;
			for (int i = 0; i < tempMesh->materials.size(); i++)
			{
				Material *pCurMaterial = &tempMesh->materials.at(i);
				if (0 == wcscmp(tempMesh->materials[i].strName, strName))
				{
					pMaterial = pCurMaterial;
					index = i;
					break;
				}
			}
		}

		// The rest of the commands rely on an active material
		if (pMaterial == NULL)
			continue;

		if (0 == wcscmp(strCommand, L"#"))
		{
			// Move on
		}
		else if (0 == wcscmp(strCommand, L"Ka"))
		{
			// Ambient color
			float r, g, b;
			inFile >> r >> g >> b;
			pMaterial->vAmbient = DirectX::XMFLOAT4(r, g, b, 1.0f);
		}
		else if (0 == wcscmp(strCommand, L"Kd"))
		{
			// Diffuse color
			float r, g, b;
			inFile >> r >> g >> b;
			pMaterial->vDiffuse = DirectX::XMFLOAT4(r, g, b, 1.0f);
		}
		else if (0 == wcscmp(strCommand, L"Ks"))
		{
			// Specular color
			float r, g, b;
			inFile >> r >> g >> b;
			pMaterial->vSpecular = DirectX::XMFLOAT4(r, g, b, 1.0f);
		}
		else if (0 == wcscmp(strCommand, L"d") ||
			0 == wcscmp(strCommand, L"Tr"))
		{
			// Alpha
			inFile >> pMaterial->fAlpha;
		}
		else if (0 == wcscmp(strCommand, L"Ns"))
		{
			// Shininess
			int nShininess;
			inFile >> nShininess;
			pMaterial->nShininess = nShininess;
		}
		else if (0 == wcscmp(strCommand, L"illum"))
		{
			// Specular on/off
			int illumination;
			inFile >> illumination;
			pMaterial->bSpecular = (illumination == 2);
		}
		else if (0 == wcscmp(strCommand, L"Ni"))
		{
			int fRefraction;
			inFile >> fRefraction;
			pMaterial->fRefraction = fRefraction;
		}
		else if (0 == wcscmp(strCommand, L"map_Kd"))
		{
			// Texture
			wchar_t textureFile[256];
			inFile >> textureFile;
			//tempMesh->textureSRV = graphics->createTextureSRV(textureFile);
		}

		else
		{
			// Unimplemented or unrecognized command, do nothing
		}

		tempMesh->materials[index] = *pMaterial;
		inFile.ignore(1000, L'\n');
	}

	inFile.close();*/

	return S_OK;
}

void MeshHandler::creatBasicLandscape(ImageInfo *image, int width, int height, int interval)
{

}

void MeshHandler::createGrid(float width, float depth, UINT n, UINT m, ImageInfo *image)
{
	UINT vertexCount = m*n;
	UINT faceCount = (m - 1)*(n - 1) * 2;
	UINT indexCount = 6 * (m - 1) * (n - 1);

	float halfWidth = .5*width;
	float halfDepth = .5*depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);
	float du = 1.0 / (n - 1);
	float dv = 1.0 / (m - 1);

	//set vertices
	image->mesh->vertices.resize(vertexCount);
	for (UINT i = 0; i < m; i++)
	{
		float z = halfDepth - i*dz;
		for (UINT j = 0; j < n; j++)
		{
			float x = halfWidth - j*dx;

			image->mesh->vertices[i*n + j].position = DirectX::XMFLOAT3(x, 0, z);
			//image->mesh->colorVertices[i*n + j].color = DirectX::XMFLOAT3(1, 0, 0);

			//fix normal and tangent for lighting later
			image->mesh->vertices[i*n + j].normal = DirectX::XMFLOAT3(0, 1, 0);
			//image->mesh->vertices[i*n + j].tangentU = DirectX::XMFLOAT3(0, 0, 0);

			//fix later for textures
			image->mesh->vertices[i*n + j].tex.x = j*du;
			image->mesh->vertices[i*n + j].tex.y = i*dv;

			//colors for now
		}
	}

	//set indices
	image->mesh->indices.resize(indexCount);
	for (UINT j = 0; j < m - 1; j++)
	{
		for (UINT i = 0; i < n - 1; i++)
		{
			image->mesh->indices[6 * i + j * (n - 1) * 6] = i + j * n;
			image->mesh->indices[6 * i + j * (n - 1) * 6 + 2] = i + 1 + (j + 1) * n;
			image->mesh->indices[6 * i + j * (n - 1) * 6 + 1] = i + (j + 1) * n;
			image->mesh->indices[6 * i + j * (n - 1) * 6 + 3] = i + j * n;
			image->mesh->indices[6 * i + j * (n - 1) * 6 + 5] = i + 1 + j * n;
			image->mesh->indices[6 * i + j * (n - 1) * 6 + 4] = i + 1 + (j + 1) * n;
		}
	}

	MaterialProperties tempMat;
	tempMat.Mat.UseTexture = true;
	image->mesh->materials.push_back(tempMat);
	
	//until texture stuff is ready
	image->color[0] = image->color[3] = 1.0;
	image->color[1] = image->color[2] = 0.0;

}

void MeshHandler::createSphere(float radius, float divisions, ImageInfo *image)
{
	int vertexCount;
	int quadDivisions = (divisions / 4);
	int verticalDivisions = (divisions / 2) + 1;

	std::vector<float> tempVerts;

	//add top vert
	tempVerts.push_back(0); tempVerts.push_back(radius); tempVerts.push_back(0);
	tempVerts.push_back((0.5 + std::atan2(0, 0) / (2.0 * PI))); //u
	tempVerts.push_back((0.5 - std::asin(1) / PI)); //v

	//add verts between top and bottom
	for (int verticalSlice = 1; verticalSlice < verticalDivisions - 1; verticalSlice++)
	{
		float verticalRadius;
		float horizontalRadius;
		if (verticalSlice < (verticalDivisions / 2.0))
		{
			//horizontalRadius = verticalSlice * (radius / quadDivisions);
			//verticalRadius = radius - horizontalRadius;
			horizontalRadius = radius * std::sin((verticalSlice * 360.0 / divisions) * PI / 180);
			verticalRadius = radius * std::cos((verticalSlice * 360.0 / divisions) * PI / 180);
		}
		else if (verticalSlice > verticalDivisions / 2.0)
		{
			//horizontalRadius = (verticalDivisions - verticalSlice - 1) * (radius / quadDivisions);
			//verticalRadius = radius - horizontalRadius;
			horizontalRadius = radius * std::sin(((verticalDivisions - verticalSlice - 1) * 360.0 / divisions) * PI / 180);
			verticalRadius = -radius * std::cos(((verticalDivisions - verticalSlice - 1) * 360.0 / divisions) * PI / 180);
		}
		else //on x-y plane
		{
			verticalRadius = 0;
			horizontalRadius = radius;
		}

		for (int horizontalSlice = 0; horizontalSlice < divisions; horizontalSlice++)
		{
			float x = horizontalRadius * (std::cos((horizontalSlice * 360.0 / divisions)* PI / 180));
			float y = verticalRadius;
			float z = horizontalRadius * (std::sin((horizontalSlice * 360.0 / divisions)* PI / 180));

			if ((x < .0001 && x > -.0001) || (x < .0001 && x > -.0001))
				x = 0;
			if ((z < .0001 && z > -.0001) || (z < .0001 && z > -.0001))
				z = 0;
			Vector3 point(x, y, z);
			Vector3 unit = point.normal();

			float u = (float)(0.5 + std::atan2(unit.z, unit.x) / (2.0 * PI));
			float v = (float)(0.5 - std::asin(unit.y) / PI);

			tempVerts.push_back(x); tempVerts.push_back(y); tempVerts.push_back(z);
			tempVerts.push_back(u); tempVerts.push_back(v);
		}
	}

	//add vert for bottom point
	tempVerts.push_back(0); tempVerts.push_back(-radius); tempVerts.push_back(0);
	tempVerts.push_back((float)(0.5 + std::atan2(0, 0) / (2.0 * PI))); //u
	tempVerts.push_back((float)(0.5 - std::asin(-1) / PI)); //v

	vertexCount = tempVerts.size() / 5;

	//calculate indices for top slice - all single triangular sections
	for (unsigned int i = 0; i < divisions; i++)
	{
		image->mesh->indices.push_back((unsigned int)0);
		image->mesh->indices.push_back((unsigned int)(i + 1));
		if (i + 2 < divisions + 1)
			image->mesh->indices.push_back((unsigned int)(i + 2));
		else
			image->mesh->indices.push_back((unsigned int)1);
	}

	//calculate indices for all middle slices - all quads split into 2 triangles
	int midSections = (int)(divisions / 2.0) - 2;

	for (int i = 1; i <= midSections; i++)
	{
		for (int j = 1; j <= divisions; j++)
		{
			//tri 1
			image->mesh->indices.push_back((unsigned int)(j + (divisions * (i - 1))));
			image->mesh->indices.push_back((unsigned int)((divisions * i) + j));
			if (j != divisions)
				image->mesh->indices.push_back((unsigned int)((divisions * i) + j + 1));
			else
				image->mesh->indices.push_back((unsigned int)((divisions * i) + 1));

			//tri 2
			if (j != divisions)
				image->mesh->indices.push_back((unsigned int)((divisions * i) + j + 1));
			else
				image->mesh->indices.push_back((unsigned int)((divisions * i) + 1));
			if (j != divisions)
				image->mesh->indices.push_back((unsigned int)((divisions * (i - 1)) + j + 1));
			else
				image->mesh->indices.push_back((unsigned int)(divisions * (i - 1) + 1));
			image->mesh->indices.push_back((unsigned int)(j + (divisions * (i - 1))));
		}
	}

	//calculate indices for bottom slice - all single triangular sections
	for (int i = 1; i <= divisions; i++)
	{
		image->mesh->indices.push_back((unsigned int)(midSections * divisions + i));
		image->mesh->indices.push_back((unsigned int)((midSections + 1) * divisions + 1));
		if ((midSections * divisions + i + 1) < ((midSections + 1) * divisions + 1))
			image->mesh->indices.push_back((unsigned int)(midSections * divisions + i + 1));
		else
			image->mesh->indices.push_back((unsigned int)(midSections * divisions + 1));
	}

	//calculate normals
	std::vector<float> tempNormals;

	//top point
	Vector3 topNormal(0, 0, 0);
	for (int i = 1; i <= (int)divisions; i++)
	{
		Vector3 edge1(tempVerts.at(i * 5) - tempVerts.at(0), tempVerts.at(i * 5 + 1) - tempVerts.at(1), tempVerts.at(i * 5 + 2) - tempVerts.at(2));
		Vector3 edge2;
		if (i != 1)
			edge2 = Vector3(tempVerts.at((i - 1) * 5) - tempVerts.at(0), tempVerts.at((i - 1) * 5 + 1) - tempVerts.at(1), tempVerts.at((i - 1) * 5 + 2) - tempVerts.at(2));
		else
			edge2 = Vector3(tempVerts.at((int)divisions * 5) - tempVerts.at(0), tempVerts.at((int)divisions * 5 + 1) - tempVerts.at(1),
			tempVerts.at((int)divisions * 5 + 2) - tempVerts.at(2));
		topNormal = topNormal.plus(edge2.cross(edge1));
	}
	topNormal = topNormal.normal();
	//add to std::vector
	tempNormals.push_back(topNormal.x); tempNormals.push_back(topNormal.y); tempNormals.push_back(topNormal.z);

	//top row points
	for (int i = 1; i <= (int)divisions; i++)
	{
		Vector3 edge1;
		Vector3 edge2;
		Vector3 edge3;
		Vector3 edge4;
		Vector3 edge5;
		Vector3 norm(0, 0, 0);

		edge1 = Vector3(tempVerts.at(i * 5 + (int)divisions * 5) - tempVerts.at(i * 5), tempVerts.at(i * 5 + (int)divisions * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at(i * 5 + (int)divisions * 5 + 2) - tempVerts.at(i * 5 + 2));
		if (i != (int)divisions)
			edge2 = Vector3(tempVerts.at((i + 1) * 5 + (int)divisions * 5) - tempVerts.at(i * 5), tempVerts.at((i + 1) * 5 + (int)divisions * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i + 1) * 5 + (int)divisions * 5 + 2) - tempVerts.at(i * 5 + 2));
		else
			edge2 = Vector3(tempVerts.at((int)divisions * 5 + 3) - tempVerts.at(i * 5), tempVerts.at((int)divisions * 5 + 4) - tempVerts.at(i * 5 + 1),
			tempVerts.at((int)divisions * 5 + 5) - tempVerts.at(i * 5 + 2));
		if (i != (int)divisions)
			edge3 = Vector3(tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5 + 2));
		else
			edge3 = Vector3(tempVerts.at(3) - tempVerts.at(i * 5), tempVerts.at(4) - tempVerts.at(i * 5 + 1),
			tempVerts.at(5) - tempVerts.at(i * 5 + 2));
		edge4 = Vector3(tempVerts.at(0) - tempVerts.at(i * 5), tempVerts.at(1) - tempVerts.at(i * 5 + 1),
			tempVerts.at(2) - tempVerts.at(i * 5 + 2));
		if (i != 1)
			edge5 = Vector3(tempVerts.at((i - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
		else
			edge5 = Vector3(tempVerts.at((int)divisions * 5) - tempVerts.at(i * 5), tempVerts.at((int)divisions * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((int)divisions * 5 + 2) - tempVerts.at(i * 5 + 2));
		norm = norm.plus(edge1.cross(edge2));
		norm = norm.plus(edge2.cross(edge3));
		norm = norm.plus(edge3.cross(edge4));
		norm = norm.plus(edge4.cross(edge5));
		norm = norm.plus(edge5.cross(edge1));
		norm = norm.normal();
		tempNormals.push_back(norm.x); tempNormals.push_back(norm.y); tempNormals.push_back(norm.z);
	}

	//mid points
	int numMids = (int)divisions / 2 - 3;
	for (int j = 1; j <= numMids; j++)
	{
		for (int i = j*(int)divisions + 1; i <= (j + 1) * (int)divisions; i++)
		{
			Vector3 edge1;
			Vector3 edge2;
			Vector3 edge3;
			Vector3 edge4;
			Vector3 edge5;
			Vector3 edge6;
			Vector3 norm(0, 0, 0);
			edge1 = Vector3(tempVerts.at(i * 5 + (int)divisions * 5) - tempVerts.at(i * 5), tempVerts.at(i * 5 + (int)divisions * 5 + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at(i * 5 + (int)divisions * 5 + 2) - tempVerts.at(i * 5 + 2));
			if (i != (j + 1) * (int)divisions)
				edge2 = Vector3(tempVerts.at((i + 1) * 5 + (int)divisions * 5) - tempVerts.at(i * 5), tempVerts.at((i + 1) * 5 + (int)divisions * 5 + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at((i + 1) * 5 + (int)divisions * 5 + 2) - tempVerts.at(i * 5 + 2));
			else
				edge2 = Vector3(tempVerts.at((int)divisions * 5 + 3) - tempVerts.at(i * 5), tempVerts.at((int)divisions * 5 + 4) - tempVerts.at(i * 5 + 1),
				tempVerts.at((int)divisions * 5 + 5) - tempVerts.at(i * 5 + 2));
			if (i != (j + 1) * (int)divisions)
				edge3 = Vector3(tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5 + 1),
				tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5 + 2));
			else
				edge3 = Vector3(tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i + 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at((i + 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
			edge4 = Vector3(tempVerts.at((i - (int)divisions) * 5) - tempVerts.at(i * 5), tempVerts.at((i - (int)divisions) * 5 + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at((i - (int)divisions) * 5 + 2) - tempVerts.at(i * 5 + 2));
			if (i != j*(int)divisions + 1)
				edge5 = Vector3(tempVerts.at((i - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at((i - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
			else
				edge5 = Vector3(tempVerts.at((i - (int)divisions - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - (int)divisions - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at((i - (int)divisions - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
			if (i != j*(int)divisions + 1)
				edge6 = Vector3(tempVerts.at((i - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at((i - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
			else
				edge6 = Vector3(tempVerts.at(((int)divisions / 2 - 2) * 5) - tempVerts.at(i * 5), tempVerts.at(((int)divisions / 2 - 2) + 1) - tempVerts.at(i * 5 + 1),
				tempVerts.at(((int)divisions / 2 - 2) + 2) - tempVerts.at(i * 5 + 2));
			norm = norm.plus(edge1.cross(edge2));
			norm = norm.plus(edge2.cross(edge3));
			norm = norm.plus(edge3.cross(edge4));
			norm = norm.plus(edge4.cross(edge5));
			norm = norm.plus(edge5.cross(edge6));
			norm = norm.plus(edge6.cross(edge1));
			norm = norm.normal();
			tempNormals.push_back(norm.x); tempNormals.push_back(norm.y); tempNormals.push_back(norm.z);
		}
	}

	//bottom row
	for (int i = vertexCount - (int)divisions - 1; i <= vertexCount - 2; i++)
	{
		Vector3 edge1;
		Vector3 edge2;
		Vector3 edge3;
		Vector3 edge4;
		Vector3 edge5;
		Vector3 norm(0, 0, 0);

		edge1 = Vector3(tempVerts.at((i - (int)divisions) * 5) - tempVerts.at(i * 5), tempVerts.at((i - (int)divisions) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i - (int)divisions) * 5 + 2) - tempVerts.at(i * 5 + 2));
		if (i != vertexCount - 2)
			edge2 = Vector3(tempVerts.at((i + 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i + 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i + 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
		else
			edge2 = Vector3(tempVerts.at((i - (int)divisions + 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - (int)divisions + 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i - (int)divisions + 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
		edge3 = Vector3(tempVerts.at((vertexCount - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((vertexCount - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((vertexCount - 1) * 5 + 1) - tempVerts.at(i * 5 + 2));
		if (i != vertexCount - (int)divisions - 1)
			edge4 = Vector3(tempVerts.at((i - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
		else
			edge4 = Vector3(tempVerts.at((i + (int)divisions - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i + (int)divisions - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i + (int)divisions - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
		if (i != vertexCount - (int)divisions - 1)
			edge5 = Vector3(tempVerts.at((i - (int)divisions - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - (int)divisions - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i - (int)divisions - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
		else
			edge5 = Vector3(tempVerts.at((i - 1) * 5) - tempVerts.at(i * 5), tempVerts.at((i - 1) * 5 + 1) - tempVerts.at(i * 5 + 1),
			tempVerts.at((i - 1) * 5 + 2) - tempVerts.at(i * 5 + 2));
		norm = norm.plus(edge1.cross(edge2));
		norm = norm.plus(edge2.cross(edge3));
		norm = norm.plus(edge3.cross(edge4));
		norm = norm.plus(edge4.cross(edge5));
		norm = norm.plus(edge5.cross(edge1));
		norm = norm.normal();
		tempNormals.push_back(norm.x); tempNormals.push_back(norm.y); tempNormals.push_back(norm.z);
	}

	//bottom point
	Vector3 botNormal(0, 0, 0);
	for (int i = 1; i <= (int)divisions; i++)
	{
		int j = i + (int)divisions * ((int)divisions / 2 - 2);
		Vector3 edge1(tempVerts.at(j * 5) - tempVerts.at((vertexCount - 1) * 5), tempVerts.at(j * 5 + 1) - tempVerts.at((vertexCount - 1) * 5 + 1),
			tempVerts.at(j * 5 + 2) - tempVerts.at((vertexCount - 1) * 5 + 2));
		Vector3 edge2;
		if (i != (int)divisions)
			edge2 = Vector3(tempVerts.at((j + 1) * 5) - tempVerts.at((vertexCount - 1) * 5), tempVerts.at((j + 1) * 5 + 1) - tempVerts.at((vertexCount - 1) * 5 + 1),
			tempVerts.at((j + 1) * 5 + 2) - tempVerts.at((vertexCount - 1) * 5 + 2));
		else
			edge2 = Vector3(tempVerts.at((j - (int)divisions - 1) * 5) - tempVerts.at((vertexCount - 1) * 5), tempVerts.at((j - (int)divisions - 1) * 5 + 1) - tempVerts.at((vertexCount - 1) * 5 + 1),
			tempVerts.at((j - (int)divisions - 1) * 5 + 2) - tempVerts.at((vertexCount - 1) * 5 + 2));
		botNormal = botNormal.plus(edge1.cross(edge2));
	}
	botNormal = botNormal.normal();
	//add to std::vector
	tempNormals.push_back(botNormal.x); tempNormals.push_back(botNormal.y); tempNormals.push_back(botNormal.z);

	//build vertices
	Vertex vert;
	for (int i = 0; i < vertexCount; i++)
	{
		vert.position = DirectX::XMFLOAT3(tempVerts.at(i * 5), tempVerts.at(i * 5 + 1), tempVerts.at(i * 5 + 2));
		vert.normal = DirectX::XMFLOAT3(tempNormals.at(i * 3), tempNormals.at(i * 3 + 1), tempNormals.at(i * 3 + 2));
		vert.tex = DirectX::XMFLOAT2(tempVerts.at(i * 5 + 3), tempVerts.at(i * 5 + 4));
		image->mesh->vertices.push_back(vert);
	}

	MaterialProperties tempMat;
	tempMat.Mat.UseTexture = true;
	image->mesh->materials.push_back(tempMat);

	//until texture stuff is ready
	image->color[0] = image->color[3] = 1.0;
	image->color[1] = image->color[2] = 0.0;
}