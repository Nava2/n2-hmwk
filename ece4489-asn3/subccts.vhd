LIBRARY ieee ;
USE ieee.std_logic_1164.all ;
PACKAGE subccts IS
	
	COMPONENT regn GENERIC ( N : INTEGER := 8 ) ;
	PORT ( R 	: IN 	STD_LOGIC_VECTOR(N-1 DOWNTO 0) ;
			 Rin, Clock	: IN 	STD_LOGIC ;
			 Q 	: OUT 	STD_LOGIC_VECTOR(N-1 DOWNTO 0) ) ;
	END COMPONENT ;
	
	COMPONENT trin GENERIC ( N : INTEGER := 8 ) ;
	PORT (	X	: IN	STD_LOGIC_VECTOR(N-1 DOWNTO 0) ;
			E	: IN	STD_LOGIC ;
			F	: OUT	STD_LOGIC_VECTOR(N-1 DOWNTO 0) ) ;
	END COMPONENT ;
	COMPONENT upcount
	PORT (	Clear, Clock	: IN		STD_LOGIC ;
			Q	: BUFFER 	STD_LOGIC_VECTOR(1 DOWNTO 0) ) ;
	END COMPONENT ;
	COMPONENT dec2to4
	PORT (	V		: IN	STD_LOGIC_VECTOR(1 DOWNTO 0) ;
			En		: IN	STD_LOGIC ;
			y		: OUT	STD_LOGIC_VECTOR(0 TO 3) );
	END COMPONENT;

	component ProcFSM 
	PORT ( Clock, Reset, w: IN STD_LOGIC;
		func: in std_logic_vector(7 downto 0) ;
		Done: INOUT STD_LOGIC;
		Extern, clrOut, cset, Ain, Aout, tempIn, tempOut, cout: OUT STD_LOGIC;
		ALUOp: out std_logic_vector(3 downto 0);
		Rin,Rout: OUT STD_LOGIC_VECTOR(0 TO 3) );
	END COMPONENT;
	
	COMPONENT ProcAccALU is
	generic ( N : integer := 8 ) ;
	PORT ( Clock, Reset 	: IN 		STD_LOGIC ;
		BusWires 	: INOUT 	STD_LOGIC_VECTOR(N-1 DOWNTO 0);
		cout			: OUT STD_LOGIC;
		temp			: in std_logic_vector(N-1 downto 0);
		AddSub, aIn, sumOut : IN STD_LOGIC ) ;
	END COMPONENT;
	
END subccts ;