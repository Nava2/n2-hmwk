----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:18:22 02/23/2015 
-- Design Name: 
-- Module Name:    ShiftRegister - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity ShiftRegister is
    GENERIC ( N : INTEGER := 4 );
    Port ( RST : in  STD_LOGIC;
           CLK : in  STD_LOGIC;
           SHIFT : in  STD_LOGIC;
           LEFTRIGHT : in  STD_LOGIC;
			  SET : in STD_LOGIC;
			  D : in STD_LOGIC_VECTOR(N-1 DOWNTO 0);
           Q : out STD_LOGIC_VECTOR(N-1 DOWNTO 0) );
end ShiftRegister;

architecture Behavioral of ShiftRegister is
	signal V : STD_LOGIC_VECTOR(N-1 downto 0);
begin
	Q <= V;
	
	PROCESS (CLK, RST, SET) BEGIN
		if (RST = '0') then
			V <= (others => '0');
		elsif (SET = '1') then 
			V <= D;
		elsif rising_edge(CLK) and (SHIFT = '1') then
			 if (LEFTRIGHT = '1') then
				V(0) <= '0';
				V(N-1 downto 1) <= V(N-2 downto 0);
			else
				V(N-1) <= '0';
				V(N-2 downto 0) <= V(N-1 downto 1);
			end if;
		end if;
	END PROCESS ;

end Behavioral;

