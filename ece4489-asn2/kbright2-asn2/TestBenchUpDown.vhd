--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   16:03:02 02/23/2015
-- Design Name:   
-- Module Name:   /home/kevin/workspace/n2-hmwk/ece4489-asn2/kbright2-asn2/TestBenchUpDown.vhd
-- Project Name:  kbright2-asn2
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: UpDown
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY TestBenchUpDown IS
END TestBenchUpDown;
 
ARCHITECTURE behavior OF TestBenchUpDown IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT UpDown
    PORT(
         UPDN : IN  std_logic;
         COUNT : IN  std_logic;
         RST : IN  std_logic;
         CLK : IN  std_logic;
         Q : OUT  std_logic_vector(3 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal UPDN : std_logic := '1';
   signal COUNT : std_logic := '0';
   signal RST : std_logic := '0';
   signal CLK : std_logic := '0';

 	--Outputs
   signal Q : std_logic_vector(3 downto 0);

   -- Clock period definitions
   constant CLK_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: UpDown PORT MAP (
          UPDN => UPDN,
          COUNT => COUNT,
          RST => RST,
          CLK => CLK,
          Q => Q
        );

   -- Clock process definitions
   CLK_process :process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
		RST <= '0';
      wait for 100 ns;	

      wait for CLK_period*10;
		RST <= '1';

      -- insert stimulus here 
		UPDN <= '1';
		COUNT <= '1';
		wait for CLK_period*4;
		
		COUNT <= '0';
		wait for CLK_period*2;
		COUNT <= '1';
		UPDN <= '0';

      wait;
   end process;

END;
