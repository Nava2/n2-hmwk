library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

USE work.subccts.all ;

entity ProcRegs is
PORT (	Clock,Reset 	: IN 		STD_LOGIC ;
			BusWires 	: INOUT 	STD_LOGIC_VECTOR(7 DOWNTO 0) ;
			Rin, Rout : IN STD_LOGIC_VECTOR(0 TO 3) ) ;
end ProcRegs;

architecture Behavioral of ProcRegs is
	SIGNAL R0, R1, R2, R3 : STD_LOGIC_VECTOR(7 DOWNTO 0) ;
begin
	reg0: regn PORT MAP ( BusWires, Rin(0), Clock, R0 ) ;
	reg1: regn PORT MAP ( BusWires, Rin(1), Clock, R1 ) ;
	reg2: regn PORT MAP ( BusWires, Rin(2), Clock, R2 ) ;
	reg3: regn PORT MAP ( BusWires, Rin(3), Clock, R3 ) ;
	tri0: trin PORT MAP ( R0, Rout(0), BusWires ) ;
	tri1: trin PORT MAP ( R1, Rout(1), BusWires ) ;
	tri2: trin PORT MAP ( R2, Rout(2), BusWires ) ;
	tri3: trin PORT MAP ( R3, Rout(3), BusWires ) ;
end Behavioral;