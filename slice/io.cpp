#include "datastructures.hpp"
#include <boost/algorithm/string.hpp>

bool objFlag()
{
	return true;
}
bool shpFlag()
{
	return true;
}


bool inputValues(variables_map vm, vector<double> &heights)
{
	vector<string> holder;
	string height = "0,1,2,3,4,5,6,7,8,9";
    
	if (vm.count("heights"))
		height = vm["heights"].as<string>();
	
	if (height.find(".txt") != string::npos)
	{
		std::ifstream fin(height);
		while (fin.good())
		{
			double temp = 0;
			fin >> temp;
			heights.push_back(temp);
		}
		fin.close();
	}
	else
	{
		holder.clear();
		boost::split(holder, height, boost::is_any_of(","), boost::token_compress_on);
		for (int i = 0; i < holder.size(); ++i)
		{
			double temp = atof(holder[i].c_str());
            if (std::find(heights.begin(), heights.end(), temp) == heights.end())
				heights.push_back(temp);
		}
	}
	return true;
}

bool readFile(boost::shared_array<double> &x,
    boost::shared_array<double> &y,
    boost::shared_array<double> &depth,
    boost::shared_array<int> &ele,
    int &node,
    int &nele,
	string input)
{

   /* Loop indexes, and error handling. */
   int retval;
   int time;
   bool hasTime = false;
   {
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( "tree.obj",
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices);
    if (!scene)
        return false;
    if (!scene->HasMeshes())
        return false;
    vector<int> indexes;
    vector<double> xvec;
    vector<double> yvec;
    vector<double> zvec;
    int total = 0;
    for (int j = 0; j < scene->mNumMeshes; ++j)
    {
   aiMesh* mesh =  scene->mMeshes[j];
   int tnode = mesh->mNumVertices;
   int tnele = mesh->mNumFaces*3;
     
     for (int i = 0; i < tnode; ++i)
     {
         xvec.push_back(mesh->mVertices[i].x);
         yvec.push_back(mesh->mVertices[i].z);
         zvec.push_back(mesh->mVertices[i].y);
     }
     for (int i = 0; i < tnele/3; ++i)
     {
         if (mesh->mFaces[i].mNumIndices == 3)
         {
             indexes.push_back(mesh->mFaces[i].mIndices[0]+total);
             indexes.push_back(mesh->mFaces[i].mIndices[1]+total);
             indexes.push_back(mesh->mFaces[i].mIndices[2]+total);
         }
     }
     total += tnode + 1;
    }
    
   node = xvec.size();
   nele = indexes.size();
	 x = boost::shared_array<double>(new double[node]);
	 y = boost::shared_array<double>(new double[node]);
	 depth = boost::shared_array<double>(new double[node]);
	 ele = boost::shared_array<int>(new int[nele]);
     
     for (int i = 0; i < node; ++i)
     {
         x[i] = xvec[i];
         y[i] = yvec[i];
         depth[i] = zvec[i];
     }
     for (int i = 0; i < nele; ++i)
     {
         ele[i] = indexes[i];
     }
}
     if (false)
   {   
	string filename = "debOutput.obj";
	std::ofstream fout(filename);
	int count = 1;
	cout << "Writing to file.\n";
	int counter = 0;
	for (int i = 0; i < nele/3; i++)
	{
		int index = i*3;
		int ind = ele.get()[index];
		double x1 = x.get()[ind];
		double y1 = y.get()[ind];
		double z1 = depth.get()[ind];
		index++;
		ind = ele.get()[index];
		double x2 = x.get()[ind];
		double y2 = y.get()[ind];
		double z2 = depth.get()[ind];
		index++;
		ind = ele.get()[index];
		double x3 = x.get()[ind];
		double y3 = y.get()[ind];
		double z3 = depth.get()[ind];
		{
			fout << "v " << x1 << " " << z1 << " " << y1 << endl;
			fout << "v " << x2 << " " << z2 << " " << y2 << endl;
			fout << "v " << x3 << " " << z3 << " " << y3 << endl;
			fout << "f " << count << " " << count + 1 << " " << count + 2 << endl;
			count += 3;
		}
	}
	fout.close();
   }
   return true;
}

void outputObj(boost::shared_array<boost::shared_ptr<vector<vector<pointxy>>>> &contours, vector<double> &heights, int nL,string output)
{
    output = "out";
	string filename = output + ".obj";
	std::ofstream fout(filename);
	int count = 1;
	cout << "Writing to file.\n";
	int counter = 0;
	for (int lin = 0; lin < nL; ++lin)
	{
		for (vector<vector<pointxy>>::iterator it = contours[lin]->begin(); it < contours[lin]->end(); ++it)
		{
			vector<pointxy> intersect = (*it);
			for (int k = 0; k < intersect.size(); k++)
			{
				fout << "v " << intersect[k].get<0>() << " " << heights[counter] << " " << intersect[k].get<1>() << endl;
			}
			fout << "g g_" << count << endl;
			for (int k = 0; k < intersect.size()-1; k++)
			{
				fout << "l " << count << " ";
				count++;
				fout << count << endl;
			}
			count++;
		}
		counter++;
	}
	fout.close();
	cout << "Written to file.\n";
}
