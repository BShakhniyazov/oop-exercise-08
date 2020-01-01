#include <sstream>  //std::stringstream
#include <map>      //std::map
#include "buffer.h"

int toInt(std::string s) //convert string to int
{
    int n = 0;
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] < '0' || s[i] > '9') throw std::logic_error("argument is not a positive integer");
        n = n * 10 + (s[i] - '0');
    }
    return n;
}
double toDouble(std::string s) //convert string to float
{
    //check string for being a floating number
    int start = 0, npoints = 0;
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == '-') //minus only as first symbol
        {
            if (i > 0) throw std::logic_error("argument is not a double");
            start = 1;
        }
        else if (s[i] == '.') //only one point and in middle
        {
            if (i == s.size() - 1 || npoints++) throw std::logic_error("argument is not a double");
        }
        else if (s[i] < '0' || s[i] > '9') throw std::logic_error("argument is not a double"); //any other non-digit is unavailable
    }
    //if it is -- convert to double
    return ::atof(s.c_str());
}
int fromName(std::string s) //get number of vertices by name
{
    if (s == "tri") return 3;
    if (s == "squ") return 4;
    if (s == "oct") return 8;
    throw std::logic_error("unknown figure " + s);
}

void help(std::vector<std::string>) //show help
{
    std::cout <<
        "quit -- выйти из программы" << std::endl <<
        "help -- показать этот текст" << std::endl <<
        "clear -- очистить буфер" << std::endl <<
        "add *figure* -- добавить фигуру" << std::endl <<
        "show -- показать фигуру" << std::endl <<
        "export -- экспортировать буфер" << std::endl <<
        "===FIGURES===" << std::endl <<
        "tri ox oy ax ay" << std::endl <<
        "squ ox oy ax ay" << std::endl <<
        "oct ox oy ax ay" << std::endl <<
        "ox,oy -- координата центра" << std::endl <<
        "ax,ay -- координата произвольной вершины" << std::endl;
}

int main(int argc, char *argv[])
{
    //arguments
    if (argc != 2)
    {
        std::cout << "Usage:" << std::endl << argv[0] << " BufferSize" << std::endl;
        return 1;
    }
    int sz = atoi(argv[1]);
    if (sz <= 0)
    {
        std::cout << "Buffer size must be positive" << std::endl;
        return 2;
    }

    Buffer<float> buffer(sz); //create buffer
    
    bool loop = true; //variable for loop
    std::string input; //inputed string

    //map of comands and number of arguments
    std::map<std::pair<std::string, int>, std::function<void(std::vector<std::string>)>> commands;

    //bind all commands
    commands[std::pair("quit", 0)] = [&loop](std::vector<std::string>){ loop = false; };
    commands[std::pair("help", 0)] = help;
    //buffer commands
    commands[std::pair("clear", 0)] = [&buffer](std::vector<std::string>){ buffer.Clear(); };
    commands[std::pair("show", 0)] = [&buffer](std::vector<std::string>) { buffer.Show(); };
    commands[std::pair("export", 0)] = [&buffer](std::vector<std::string>) { buffer.Export(); };
    commands[std::pair("add", 5)] = [&buffer](std::vector<std::string> v)
    {
        buffer.Add(fromName(v[0]), toDouble(v[1]), toDouble(v[2]), toDouble(v[3]), toDouble(v[4]));
    };

    //start UI
    std::cout <<
        "+---------------------------------+" << std::endl <<
        "|Welcome to the figure calculator!|" << std::endl <<
        "+---------------------------------+" << std::endl <<
        "Для вызова помощи введите help" << std::endl;
    
    while (loop)
    {
        std::cout << "> ";
        std::getline(std::cin, input); //read uer input

        //split string
        std::stringstream ss(input);
        std::vector<std::string> words;
        for (std::string s; ss >> s; ) //split into words
        {
            for (int i = 0; i < s.size(); i++) s[i] = std::tolower(s[i]); //set register to lower
            words.push_back(s);
        }
        if (words.size() == 0) continue;

        auto iter = commands.find(std::pair(words[0], words.size() - 1));
        if (iter == commands.end()) //not a command in map
        {
            std::cout << "unknowm command \"" << words[0] << "\" with " << words.size() - 1 << " arguments" << std::endl;
            continue;
        }
        //get command data
        auto com = (*iter).second;
        //execute
        words.erase(words.begin());
        try
        {
            com(words);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    return 0;
}
