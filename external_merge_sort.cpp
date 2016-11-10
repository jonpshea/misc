//Jon Shea
//11/10/2016
//Started at 4:20 PM
//Finished at 6:20 PM

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<algorithm>
#include<queue>

using namespace std;

const string TEMP_FILE_NAME = "temp";
const int CHUNK_SIZE = 2500;

int s_count(string str)  //Count the number of s's in a line
{
    int ct = 0;
    for (char c : str)
    {
        if (c == 's' || c == 'S')
            ct++;
    }

    return ct;
}

bool s_comparator(pair<string, int> &s1, pair<string,int> &s2)       //Comparator for sort() for s counts
{
    return s1.second < s2.second;
}

struct comparator{                                                   //Comparator for min heap
    bool operator()(pair<string, int> &s1, pair<string, int> &s2) {
        return s_count(s2.first) < s_count(s1.first);
    }
};

//Helper function for external_merge
//Merge all of the temporary input files
void merge_temp_files(ofstream &output, int merges, string temp_file_pred)
{
    vector<ifstream> input_files(merges);
    string temp_str;
    int idx = 0;
    priority_queue<pair<string, int>, vector<pair<string, int>>, comparator> cur_values;   //For storing one line from each temp file

    for (int i = 0; i < merges; i++)                                                       //Open all of the files to merge
    {
        input_files[i] = ifstream(temp_file_pred + to_string(i));
        if (!input_files[i].is_open())
        {
            cout << "Temporary file " << temp_file_pred + to_string(i) << " not found" << endl;
            return;
        }
        getline(input_files[i], temp_str);                        //Put first line from sorted chunk into our min-heap
        cur_values.push(pair<string, int>(temp_str, i));
    }

    // Use a min heap to keep the lines with the least number of s's on top.
    while (!cur_values.empty())
    {
        output << (cur_values.top()).first << endl;               //output line with least s's
        idx = (cur_values.top()).second;
        cur_values.pop();
        if (getline(input_files[idx], temp_str))                  //get next line from file
            cur_values.push(pair<string, int>(temp_str, idx));
    }

    for (int i = 0; i < merges; i++) // Clean up -- close input streams and output stream
    {
        input_files[i].close();
        output.close();       
    }
}

//Helper function for external merge
//Delete temporary files
void remove_temp_files(int num_files, string temp_file_prod)
{
    for (int i = 0; i <= num_files; i++)
        remove((temp_file_prod + to_string(i)).c_str());
}

//Load chunk_size input lines from input, sort them using stl sort, output to a temp file
void external_merge(ifstream &input, ofstream &output, int chunk_size)
{
    vector<pair<string, int>> buffer; //100 lines
    string inp;
    int filenum = 0;
    int lines = 0;
    string filename = TEMP_FILE_NAME + to_string(filenum);

    //Create partitions, in chunks of 100
    while (getline(input, inp)) //read in input, sort, print to temp files
    {
        buffer.push_back(pair<string, int>(inp, s_count(inp)));
        lines++;
        if (lines == CHUNK_SIZE)
        {
            sort(buffer.begin(), buffer.end(), s_comparator);
            ofstream out(filename);

            for (auto s : buffer)
                out << s.first << endl;
            buffer.clear();

            //clean up and set values for creating next partition
            out.close();
            lines = 0;
            filenum++;
            filename = TEMP_FILE_NAME + to_string(filenum);
        }
    }
    if (lines != 0) //put remaining lines in final partition
    {
        sort(buffer.begin(), buffer.end(), s_comparator);
        ofstream out(filename);

        for (auto s : buffer)
            out << s.first << endl;

        out.close();
        buffer.clear();
        filenum++;

    }

    input.close();

    merge_temp_files(output, filenum, TEMP_FILE_NAME);
    remove_temp_files(filenum, TEMP_FILE_NAME);
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Please specify input and output file." << endl;
        return 0;
    }

    ifstream input(argv[1]);    //Open input stream
    ofstream output(argv[2]);   //Open output stream

    //Make sure our input stream opened.
    if (!input.is_open())
        cout << "Unable to open input file \"" << argv[1] << "\"" << endl;
    else if (!output.is_open())
        cout << "Unable to open output file \"" << argv[2] << "\"" << endl;
    else
        external_merge(input, output, CHUNK_SIZE);  //Sort the input and write it to output.

    cout << argv[1] << " was sorted successfully with output written to " << argv[2] << endl;

    system("pause");
    return 0;
}