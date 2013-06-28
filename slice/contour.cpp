#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/util.hpp>
#include <hpx/include/lcos.hpp>

#include "datastructures.hpp"

bool inputValues(variables_map vm, vector<double> &heights);

void outputObj(boost::shared_array<boost::shared_ptr<vector<vector<pointxy>>>> &contours, vector<double> &heights, int nL,string output);
bool readFile(boost::shared_array<double> &x,
    boost::shared_array<double> &y,
    boost::shared_array<double> &depth,
    boost::shared_array<int> &ele,
    int &node,
    int &nele,
	string input);
int retLine(double current, int size,
	boost::shared_array<double> &x, boost::shared_array<double> &y, boost::shared_array<double> &depth,
	boost::shared_array<int> &ele, boost::shared_ptr< vector <vector <pointxy> >>& contours);


void outputLines(string input, string output, vector<string> attnames, vector<double> heights, int timestep, bool outputInput, string xs, string ys)
{
    size_t const os_threads = hpx::get_os_thread_count();
    cout << "Program will run on " << os_threads << " threads.\n";
	int nL = heights.size();
	cout << "Reading in file.\n";


   /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
	 
	boost::shared_array<double> x;
	boost::shared_array<double> y;
	boost::shared_array<double> depth;
	boost::shared_array<int> ele;
	 int node = 0;
	 int nele = 0;
	 if (!readFile(x,y,depth,ele,node,nele, input))
	 {
		 cout << "File read error. Check input filename and validity of input data.\n";
		 return;
	 }
    cout << "File read. Starting timer and generating contour lines.\n";
	hpx::util::high_resolution_timer t;
		
	boost::shared_array<boost::shared_ptr<vector<vector<pointxy>>>> contours =
		boost::shared_array<boost::shared_ptr<vector<vector<pointxy>>>>(new boost::shared_ptr<vector<vector<pointxy>>>[nL]);
   {
		using hpx::lcos::future;
		using hpx::async;
		vector<future<int>> waiting;
	   for (int i = 0; i < heights.size(); ++i)
	   {
		   contours[i] = boost::shared_ptr<vector<vector<pointxy>>>(new vector<vector<pointxy>>());
		   waiting.push_back(async(&retLine,heights[i], nele/3, x,y,depth,ele,contours[i]));
	  }
	   hpx::wait_all(waiting);
   }
	double ti = t.elapsed();
	cout << "Program completed in " << ti << "s.\n";
	std::ofstream fout("times.txt", std::ios_base::app);
	fout << os_threads << " " << ti << endl;
	fout.close();
	outputObj(contours, heights, nL,output);
	return;
}

int hpx_main(variables_map& vm)
{
	string input, output;
	vector<string> attnames;
	vector<double> heights;
	string xs, ys;
	int timestep;
  // Configure application-specific options.
	bool outputInput = false;
	if (!inputValues(vm, heights))
	{
		hpx::finalize(); // Handles HPX shutdown
		return 1;
	}
	outputLines(input,output,attnames,heights,timestep, outputInput,xs, ys);
	hpx::finalize(); // Handles HPX shutdown
	return 0;
}

int main(int argc, char *argv[])
{
	options_description desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");
	desc_commandline.add_options()
    ("input", value<string>(), "Set input file\n")
    ("heights", value<string>()->default_value("0,1,2,3,4,5,6,7,8,9"), "Heights to generate the contours from. Mulitple heights are separated by a comma (,)\n")
    ;
	return hpx::init(desc_commandline, argc, argv);
}
