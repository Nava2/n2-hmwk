--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   16:20:50 02/23/2015
-- Design Name:   
-- Module Name:   /home/kevin/workspace/n2-hmwk/ece4489-asn2/kbright2-asn2/ShiftRegisterTestBench.vhd
-- Project Name:  kbright2-asn2
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: ShiftRegister
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
 
ENTITY ShiftRegisterTestBench IS
END ShiftRegisterTestBench;
 
ARCHITECTURE behavior OF ShiftRegisterTestBench IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT ShiftRegister
    PORT(
         RST : IN  std_logic;
         CLK : IN  std_logic;
         SHIFT : IN  std_logic;
         LEFTRIGHT : IN  std_logic;
         SET : IN  std_logic;
			D : IN std_logic_vector(3 downto 0);
         Q : OUT  std_logic_vector(3 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal RST : std_logic := '0';
   signal CLK : std_logic := '0';
   signal SHIFT : std_logic := '0';
   signal LEFTRIGHT : std_logic := '0';
   signal SET : std_logic := '0';
	signal D : std_logic_vector(3 downto 0) := (others => '1');

 	--Outputs
   signal Q : std_logic_vector(3 downto 0);

   -- Clock period definitions
   constant CLK_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: ShiftRegister PORT MAP (
          RST => RST,
          CLK => CLK,
          SHIFT => SHIFT,
          LEFTRIGHT => LEFTRIGHT,
          SET => SET,
			 D => D,
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
		RST <= '0';
      -- hold reset state for 100 ns.
      wait for 100 ns;	

		RST <= '1';
		SET <= '1';

      wait for CLK_period*10;
		
		SET <= '0';
		wait for CLK_period*2;
		
		SHIFT <= '1';
		LEFTRIGHT <= '1';
		wait for CLK_period*2; -- Q == "1100"
		
		LEFTRIGHT <= '0';
		wait for CLK_period*3; -- Q == "0001"
		SHIFT <= '0';
		
		-- hold indefinitely

      wait;
   end process;

END;
