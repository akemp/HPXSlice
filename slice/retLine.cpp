#include "datastructures.hpp"
#include "treeHeaders.hpp"


pointxy intRatio (model::point<double, 3, cs::cartesian> p1, model::point<double, 3, cs::cartesian> p2, double height, int& intersects)
{
    double z1 = get<2>(p1), z2 = get<2>(p2);
    intersects = 1;
    pointxy seg;
    if (z1 >= height && z2 <= height)
    {
        if (z1 == z2)
            assign_values(seg, get<0>(p1), get<1>(p1));
        else
        {
            double range = z1-z2;
            double diff1 = z1-height;
            double ratio1 = 1-diff1/range;
            double diff2 = height-z2;
            double ratio2 = 1-diff2/range;
            double x = get<0>(p1)*ratio1+get<0>(p2)*(ratio2);
            double y = get<1>(p1)*ratio1+get<1>(p2)*(ratio2);
            assign_values(seg, x, y);
        }
    }
    else if (z2 >= height && z1 <= height)
    {
        if (z1 == z2)
            assign_values(seg, get<0>(p1), get<1>(p1));
        else
        {
            double range = z2-z1;
            double diff1 = z2-height;
            double ratio1 = 1-diff1/range;
            double diff2 = height-z1;
            double ratio2 = 1-diff2/range;
            double x = get<0>(p1)*ratio2+get<0>(p2)*(ratio1);
            double y = get<1>(p1)*ratio2+get<1>(p2)*(ratio1);
            assign_values(seg, x, y);
        }
    }
    else
    {
        assign_values(seg, 0, 0);
        intersects = 0;
    }
    return seg;
}
static bool point2Equals(pointxy p1, pointxy p2)
{
	return ((get<0>(p1) == get<0>(p2)) && (get<1>(p1) == get<1>(p2)));
}
vector<pointxy> planeInter(double height,vector<double> input)
{
    
    
    model::point<double, 3, cs::cartesian> p1;
    assign_values(p1, input[0], input[1], input[2]);
    model::point<double, 3, cs::cartesian> p2;
    assign_values(p2, input[3], input[4], input[5]);
    model::point<double, 3, cs::cartesian> p3;
    assign_values(p3, input[6], input[7], input[8]);
    //model::point<double, 3, cs::cartesian> p4;
    //assign_values(p4, input[0], input[1], input[2]);
    //append(this->poly,this->points);
    
    vector<pointxy > s;
    int intersects = 0;
    pointxy seg = intRatio(p1,p2,height,intersects);
    if (intersects)
        s.push_back(seg);
    seg = intRatio(p2,p3,height,intersects);
    if (intersects)
        s.push_back(seg);
    seg = intRatio(p3,p1,height,intersects);
    if (intersects)
        s.push_back(seg);
    vector<pointxy > final;
    for (int i = 0; i < s.size(); i++)
    {
        bool contains = false;
        pointxy cur = s[i];
        for (int j = i+1; j < s.size(); j++)
        {
            if (point2Equals(s[j],cur))
            {
                contains = true;
                break;
            }
        }
        if (!contains)
            final.push_back(cur);
    }
    return final;
}

int retLine(double current, int size,
	boost::shared_array<double> &x, boost::shared_array<double> &y, boost::shared_array<double> &depth,
	boost::shared_array<int> &ele, boost::shared_ptr< vector <vector <pointxy> >>& contours)
{
	for (int i = 0; i < size; ++i)
	{
		int index = i*3;
		vector<double> stores;
		stores.reserve(9);
		bool use = true;
		for (int j = 0; j < 3; ++j)
		{
			int place = ele[index];
			stores.push_back(x[place]);
			stores.push_back(y[place]);
			stores.push_back(depth[place]);
			index++;
		}
		if (use)
		{
			vector<pointxy> line = planeInter(current,stores);
			if (line.size() >= 2)
				contours->push_back(line);
		}
	}
	
	double margin = 0.001;

	{
		bool found = true;
		while (found)
		{
			found = false;
			std::set<int> setter;
			vector<pointxy> pointsf;
			const int size = contours->size();
			pointsf.reserve(size);
			for (vector<vector<pointxy>>::iterator it = contours->begin(); it < contours->end(); ++it)
			{
				pointsf.push_back(it->front());
			}
			const kd_tree treef(pointsf);
			vector<vector<pointxy>> templ;
			int i = 0;
			for (vector<vector<pointxy>>::iterator it2 = contours->begin(); it2 < contours->end(); ++it2)
			{
				std::set<int>::iterator it = setter.find(i);
				if (it == setter.end())
				{
					vector<pointxy> holder = *it2;
					bool smallFound = true;
						int tempInd = i;
					while (smallFound)
					{
						smallFound = false;
						pointxy start = holder.front();
						pointxy end = holder.back();
						int indf = treef.nearest(start,pointsf,tempInd,margin);
						it = setter.find(indf);
						if (indf == -1 || it != setter.end())
						{
							indf = treef.nearest(end,pointsf,tempInd,margin);
							it = setter.find(indf);
						}
						if (indf != -1 && it == setter.end())
						{
							found = true;
							vector<pointxy> temp = (*contours)[indf];
							double dist1 = boost::geometry::distance(start, temp.front());
							double dist2 = boost::geometry::distance(end, temp.front());
					
							if (dist2 < dist1)
							{
								double dist = boost::geometry::distance(holder.back(), temp.front());
#ifdef ERROR_CHECK
								if (dist > margin*2)
									cout << "Error 1-1: Distance too great!\n";
#endif
								holder.insert(holder.end(), temp.begin()+1, temp.end());
								if (indf > tempInd)
								{
									tempInd = indf;
									smallFound = true;
								}
							}
							else
							{
								reverse(holder.begin(), holder.end());
								double dist = boost::geometry::distance(holder.back(), temp.front());
#ifdef ERROR_CHECK
								if (dist > margin*2)
									cout << "Error 1-2: Distance too great!\n";
#endif
								holder.insert(holder.end(), temp.begin()+1, temp.end());
								if (indf > tempInd)
								{
									tempInd = indf;
									smallFound = true;
								}
							}
							setter.insert(indf);
						}
					}
				templ.push_back(holder);
				}
			++i;
			}
			contours->assign(templ.begin(), templ.end());
		}
	}
	  
	{
		bool found = true;
		while (found)
		{
			found = false;
			std::set<int> setter;
			vector<pointxy> pointsf;
			const int size = contours->size();
			pointsf.reserve(size);
			for (vector<vector<pointxy>>::iterator it = contours->begin(); it < contours->end(); ++it)
			{
			pointsf.push_back(it->back());
			}
			const kd_tree treef(pointsf);
			vector<vector<pointxy>> templ;
			int i = 0;
			for (vector<vector<pointxy>>::iterator it2 = contours->begin(); it2 < contours->end(); ++it2)
			{
				std::set<int>::iterator it = setter.find(i);
				if (it == setter.end())
				{
					vector<pointxy> holder = *it2;
					bool smallFound = true;
						int tempInd = i;
					while (smallFound)
					{
						smallFound = false;
						pointxy start = holder.front();
						pointxy end = holder.back();
						int indf = treef.nearest(start,pointsf,tempInd,margin);
						it = setter.find(indf);
						if (indf == -1 || it != setter.end())
						{
							indf = treef.nearest(end,pointsf,tempInd,margin);
							it = setter.find(indf);
						}
						if (indf != -1 && it == setter.end())
						{
							found = true;
							vector<pointxy> temp = (*contours)[indf];
							reverse(temp.begin(), temp.end());
							double dist1 = boost::geometry::distance(start, temp.front());
							double dist2 = boost::geometry::distance(end, temp.front());
					
							if (dist2 < dist1)
							{
								double dist = boost::geometry::distance(holder.back(), temp.front());
#ifdef ERROR_CHECK
								if (dist > margin*2)
									cout << "Error 2-1: Distance too great!\n";
#endif
								holder.insert(holder.end(), temp.begin()+1, temp.end());
								if (indf > tempInd)
								{
									tempInd = indf;
									smallFound = true;
								}
							}
							else
							{
								reverse(holder.begin(), holder.end());
								double dist = boost::geometry::distance(holder.back(), temp.front());
#ifdef ERROR_CHECK
								if (dist > margin*2)
									cout << "Error 2-2: Distance too great!\n";
#endif
								holder.insert(holder.end(), temp.begin()+1, temp.end());
								if (indf > tempInd)
								{
									tempInd = indf;
									smallFound = true;
								}
							}
							setter.insert(indf);
						}
					}
				templ.push_back(holder);
				}
				++i;
			}
			contours->assign(templ.begin(), templ.end());
		}
	}
	return 1;
}
