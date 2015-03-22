LIBRARY ieee ;
USE ieee.std_logic_1164.all ;
USE ieee.numeric_std.all ;
USE work.subccts.all ;
ENTITY Proc IS
	PORT (	Data 		: IN 		STD_LOGIC_VECTOR(7 DOWNTO 0) ;
			Reset, w 	: IN 		STD_LOGIC ;
			Clock 		: IN 		STD_LOGIC ;
			func			: IN 		std_logic_vector(7 downto 0) ;
			Done 			: INOUT 	STD_LOGIC ;
			BusWires 	: INOUT 	STD_LOGIC_VECTOR(7 DOWNTO 0) ) ;
END proc ;

ARCHITECTURE Behavior2 OF Proc IS
	SIGNAL X, Y, Rin, Rout : STD_LOGIC_VECTOR(0 TO 3) ;
	SIGNAL Clear, AddSub : STD_LOGIC ;
	SIGNAL Extern, Ain, tempIn, tempOut, FRin, aOut : STD_LOGIC ;
	SIGNAL R0, R1, R2, R3 : STD_LOGIC_VECTOR(7 DOWNTO 0) ;
	SIGNAL A, Sum, temp : STD_LOGIC_VECTOR(7 DOWNTO 0) ;
	SIGNAL Sel : STD_LOGIC_VECTOR(1 TO 7) ;
	
	signal cVec, coutVec: std_logic_vector(0 to 0);
	signal carry, cin, cout: std_logic;
BEGIN	
	reg0: regn PORT MAP ( BusWires, Rin(0), Clock, R0 ) ;
	reg1: regn PORT MAP ( BusWires, Rin(1), Clock, R1 ) ;
	reg2: regn PORT MAP ( BusWires, Rin(2), Clock, R2 ) ;
	reg3: regn PORT MAP ( BusWires, Rin(3), Clock, R3 ) ;
	regA: regn PORT MAP ( BusWires, Ain, Clock, A ) ;
	regCarry: regn generic map ( N => 1) port map (cVec, cin, Clock, coutVec);
	
	cVec(0) <= carry;
	cout <= coutVec(0);
	
	alu: process(AddSub, A, BusWires) 
		variable add, sub: std_logic_vector(8 downto 0);
	begin
		add := std_logic_vector(signed(A(A'high) & A) + signed(BusWires(BusWires'high) & BusWires));
		sub := std_logic_vector(signed(A(A'high) & A) - signed(BusWires(BusWires'high) & BusWires));
		
		case AddSub is
			when '0' =>
				Sum <= add(7 downto 0);
				carry <= add(add'high);
			WHEN others =>
				Sum <= sub(sub'high-1 downto 0);
				carry <= sub(sub'high);
		end case;
	end process;
	
	regTemp: regn PORT MAP ( Sum, tempIn, Clock, temp ) ; 
	Sel <= Rout & tempOut & aOut & Extern ;
	WITH Sel SELECT
		BusWires <= R0 	WHEN "1000000",
					R1 		WHEN "0100000",
					R2 		WHEN "0010000",
					R3 		WHEN "0001000",
					temp		WHEN "0000100",
					A			WHEN "0000010",
					Data 	WHEN OTHERS ;
					
	ControlCircuit: ProcFSM PORT MAP(Clock, Reset, w, cout, func,
			Done, Extern, Ain, aOut, tempIn, tempOut, AddSub, cin, Rin, Rout);

END Behavior2 ;