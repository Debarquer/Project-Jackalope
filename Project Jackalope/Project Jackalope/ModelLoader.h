#pragma once
class ModelLoader
{
private:
public:
	ModelLoader();
	~ModelLoader();

	bool LoadTextFile();
	bool LoadBinaryFile();
	bool ConvertTextToBinary();
};

