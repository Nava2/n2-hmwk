LIBRARY ieee ;
USE ieee.std_logic_1164.all ;
use ieee.numeric_std.all;
USE work.subccts.all ;

entity ProcAccALU is
generic ( N : integer := 8 ) ;
PORT ( Clock, Reset 	: IN 		STD_LOGIC ;
	BusWires 	: INOUT 	STD_LOGIC_VECTOR(N-1 DOWNTO 0);
	cout			: OUT STD_LOGIC;
	temp			: in std_logic_vector(N-1 downto 0);
	AddSub, aIn, sumOut : IN STD_LOGIC ) ;
end ProcAccALU;

architecture Behavioral of ProcAccALU is
	signal a, sum : STD_LOGIC_VECTOR(N-1 DOWNTO 0) ;
BEGIN
	regA: regn PORT MAP ( BusWires, Ain, Clock, a ) ;
	
	alu: process(AddSub, a, temp)
		variable a_minus_bus, a_plus_bus: std_logic_vector(N downto 0);
	begin
		a_minus_bus := std_logic_vector(signed(a(a'high) & a) - signed(temp(temp'high) & temp));
		a_plus_bus  := std_logic_vector(signed(a(a'high) & a) + signed(temp(temp'high) & temp));
		
		case AddSub is
			when '0' => 
				sum <= a_plus_bus(a_plus_bus'high-1 downto 0);
				cout <= a_plus_bus(a_plus_bus'high);
			when others =>
				sum <= a_minus_bus(a_plus_bus'high-1 downto 0);
				cout <= a_minus_bus(a_minus_bus'high);
		end case;		
	end process;
				
	triOut: trin PORT MAP ( sum, sumOut, BusWires ) ;
end Behavioral;