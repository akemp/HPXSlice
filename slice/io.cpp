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
   std::ifstream fin("bin.obj");
   if (!fin.is_open())
	   return false;
   while (fin.good())
   {
       char* in = new char[10000];
       fin.getline(in,10000);
       ++node;
   }
   nele = (node*6)/4;
   fin.close();
   fin.open("bin.obj");
   /* Get the varid of the data variable, based on its name. */

	 x = boost::shared_array<double>(new double[node]);
	 y = boost::shared_array<double>(new double[node]);
	 depth = boost::shared_array<double>(new double[node]);
	 ele = boost::shared_array<int>(new int[nele]);
     int count = 0;
     while (fin.good() && count < node)
     {
         fin >> x[count] >> depth[count] >> y[count];
         count++;
     }
     for (int i = 0; i < count/6 && i < nele/6; ++i)
     {
         int index = i*6;
         int place = i*4;
         ele[index] = place;
         ele[index+1] = place+1;
         ele[index+2] = place+2;
         ele[index+3] = place;
         ele[index+4] = place+2;
         ele[index+5] = place+3;
     }
     if (false)
   {   
	string filename = "debOutput.obj";
	std::ofstream fout(filename);
	count = 1;
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
