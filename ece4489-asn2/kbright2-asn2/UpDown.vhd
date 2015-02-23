----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:02:21 02/23/2015 
-- Design Name: 
-- Module Name:    UpDown - Behavioral 
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
use IEEE.numeric_std.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity UpDown is
    GENERIC ( N : INTEGER := 4);
    Port ( UPDN : in  STD_LOGIC;
           COUNT : in  STD_LOGIC;
           RST : in  STD_LOGIC;
			  CLK : IN STD_LOGIC;
           Q : out  STD_LOGIC_VECTOR(N-1 DOWNTO 0) );
end UpDown;

architecture Behavioral of UpDown is
	signal V : STD_LOGIC_VECTOR(N-1 DOWNTO 0);
begin
	Q <= V;

	PROCESS ( RST, CLK )
	BEGIN
		IF RST = '0' THEN
			V <= (OTHERS => '0') ;
		ELSIF rising_edge(CLK) and COUNT = '1' THEN
			IF UPDN = '1' THEN
				V <= STD_LOGIC_VECTOR(unsigned(V) + 1);
			ELSE
				V <= STD_LOGIC_VECTOR(unsigned(V) - 1);
			END IF;
		END IF ;
	END PROCESS ;

end architecture Behavioral;

