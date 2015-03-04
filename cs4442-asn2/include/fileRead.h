#ifndef __fileRead_H
#define __fileRead_H


#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <functional>
#include <vector>

#define EOS  "<END>"


// for reading characters or words (strings) from file
// when reading words(strings), if end of sentence is detected
// output a special string END_OF_SENTENCE = "<END>"


class fileRead {
public:
    fileRead(std::string fName);
	~fileRead();
    void readCharTokens(std::vector<std::string> &tokens);
    void readStringTokens(std::vector<std::string> &tokens);
    void readStringTokensEOS(std::vector<std::string> &tokens);

private:
	FILE *m_stream;
	bool m_EOS;
	bool m_LastWordReturnedEOS;
	bool m_readEOS;
	bool m_readChar;

    std::string readStringWithEOS();
    std::string readStringWithoutEOS();
    std::string readString();
};

/////////////////////////////////////////////////////////////////////
// Class for handling errors 
/////////////////////////////////////////////////////////////////////

class FileReadException
{
public:
    const std::string message;
    FileReadException( const std::string& m ) : message(m) {  }

    void Report() {
        std::cout << std::endl << this->message << std::endl;
	}
};

//////////////////////////////////////////////////////////////////////

inline void read_tokens(const std::string& filename, std::vector<std::string>& tokens, bool eos) {
	fileRead fr(filename);  // open file for reading 
	if (eos)
		fr.readStringTokensEOS(tokens);  //  reads string tokens with EOS marker
	else
		fr.readStringTokens(tokens);        //  reads string tokens without EOS marker
}

//////////////////////////////////////////////////////////////////////

inline void read_tokens(const std::string& filename, std::vector<char>& tokens, bool latin_only) {
    std::ifstream fr(filename);
	for (char token; fr >> token;)
		if (!latin_only )
			tokens.push_back(token);
        else  if (  (('a' <= token) && (token <= 'z')) || (('A' <= token) && (token <= 'Z')) ){
            if ( (token >= 'A') && (token <= 'Z') )
                token += 32; // make capitals lowercase
			tokens.push_back(token);
		}
}

#endif

