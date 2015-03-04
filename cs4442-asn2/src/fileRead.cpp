#include "fileRead.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////
//                       class fileRead   methods                              //
/////////////////////////////////////////////////////////////////////////////////


fileRead::fileRead(string fName)
{

	m_stream = fopen(fName.c_str(),"r");

	// Check if can open the file
	if ( m_stream == NULL ) throw FileReadException("Cannot open file. Check the file name.");

	m_EOS = false;
	m_LastWordReturnedEOS=false;
	m_readEOS = false;
	m_readChar = false;
};



/////////////////////////////////////////////////////////////////////////////////

fileRead::~fileRead()
{
	fclose(m_stream);
}

/////////////////////////////////////////////////////////////////////////////////


string fileRead::readString()
{
	if ( m_readEOS )
		return readStringWithEOS();
	else if ( m_readChar)
	{
		string s;
		char ch = fgetc( m_stream );
		if (ch != EOF)
			s.append(1,ch);;
		return(s);
	}
	return readStringWithoutEOS();
}


/////////////////////////////////////////////////////////////////////////////////

string fileRead::readStringWithEOS()
{
	string toReturn;

	if ( m_EOS ) {
		m_EOS = false;
		m_LastWordReturnedEOS = true;
		return(EOS);
	}

	// if file is finished, first return EOS if previous word was not EOS
	// then an empty string. If previous word was EOS return an empty string
	if ( feof(m_stream) != 0 ){
		if ( m_LastWordReturnedEOS == false )
		{
			m_LastWordReturnedEOS = true;
			return(EOS);
		}
		return(toReturn);
	}
	
	// first read file characters until find a character in letter range
	// or an end of sentence character
	char ch = fgetc( m_stream );
	while  ( !((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) ) 
	{
		// end of sentence
		if ( ch == '.' || ch == '!' || ch == '?' || ch == 34 )
		{
			if ( m_LastWordReturnedEOS == false )
			{
				m_LastWordReturnedEOS = true;
				return(EOS);
			}
		}

		if ( feof(m_stream) != 0 )
		{
			if ( m_LastWordReturnedEOS == false )
			{
				m_LastWordReturnedEOS = true;
				return(EOS);
			}
			return(toReturn);
		}
		ch = fgetc( m_stream );
	}

	// now get a concequitive sequence of letter characters
	while  ( ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) ) 
	{
		if ( (ch >= 'A' && ch <= 'Z') ) ch = ch + 32; // make capitals lowercase
			toReturn.append(1,ch);

		if ( feof(m_stream) != 0 ){
			m_LastWordReturnedEOS = false;
			return(toReturn);
		}
		ch = fgetc( m_stream );
	}

	// check if last character, which would be  non-letter character, is end of sentence 
	if ( ch == '.' || ch == '!' || ch == '?' || ch == 34 )
		m_EOS = true;

	m_LastWordReturnedEOS = false;
	return(toReturn);
}

/////////////////////////////////////////////////////////////////////////////////


string fileRead::readStringWithoutEOS()
{
	string toReturn;


	// if file is finished, return empty string
	if ( feof(m_stream) != 0 ){
		return(toReturn);
	}
	
	// first read file characters until find a character in letter range
	// or an end of sentence character
	char ch = fgetc( m_stream );
	while  ( !((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) ) 
	{

		if ( feof(m_stream) != 0 )
		{
			return(toReturn);
		}
		ch = fgetc( m_stream );
	}

	// now get a concequitive sequence of letter characters
	while  ( ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) ) 
	{
		if ( (ch >= 'A' && ch <= 'Z') ) ch = ch + 32; // make capitals lowercase
			toReturn.append(1,ch);

		if ( feof(m_stream) != 0 ){
			return(toReturn);
		}
		ch = fgetc( m_stream );
	}

	return(toReturn);
}


////////////////////////////////////////////////////////////////////////////////


void fileRead::readStringTokens(vector<string> &tokens)
{
    tokens.clear();
	
	string s;
	s.assign(readString()); // read next string from file into s

	// if no more strings left in the file, fr returns an empty string
	while ( s.size() != 0 )
	{
		tokens.push_back(s);
		s.assign(readString());  // read next string from file into s
	}
}

////////////////////////////////////////////////////////////////////////////////

void fileRead::readStringTokensEOS(vector<string> &tokens)
{
	 m_readEOS = true;
	 readStringTokens(tokens);   
}


////////////////////////////////////////////////////////////////////////////////

void fileRead::readCharTokens(vector<string> &tokens)
{
	 m_readChar = true;
	 readStringTokens(tokens);
    
}
