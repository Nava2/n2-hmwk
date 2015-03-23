--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   11:03:25 03/21/2015
-- Design Name:   
-- Module Name:   /home/kevin/workspace/n2-hmwk/ece4489-asn3/ProcTB1.vhd
-- Project Name:  kbright2-asn3
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: proc
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
 
ENTITY ProcTB1 IS
END ProcTB1;
ARCHITECTURE behavior OF ProcTB1 IS 
	COMPONENT proc
	PORT(
		Data : IN  std_logic_vector(7 downto 0);
		Reset : IN  std_logic;
		w : IN  std_logic;
		Clock : IN  std_logic;
		func	: IN  std_logic_vector(7 downto 0);
		Done : INOUT  std_logic;
		BusWires : INOUT  std_logic_vector(7 downto 0)
	  );
	END COMPONENT;
	
	--Inputs
	signal Data : std_logic_vector(7 downto 0) := (others => '0');
	signal Reset : std_logic := '0';
	signal w : std_logic := '0';
	signal Clock 	: std_logic := '0';
	signal F 		: std_logic_vector(7 downto 0) := (others => '0');
	signal BusWires : std_logic_vector(7 downto 0);
	
	--Outputs
	signal Done : std_logic;
	
	-- Clock period definitions
	constant Clock_period : time := 50 ns;
BEGIN
	uut: proc PORT MAP (
			 Data => Data,
			 Reset => Reset,
			 w => w,
			 Clock => Clock,
			 func => F,
			 Done => Done,
			 BusWires => BusWires
		  );
	Clock_process: process
	begin
		Clock <= '0';
		wait for Clock_period/2;
		Clock <= '1';
		wait for Clock_period/2;
	end process;

	-- Stimulus process
	stim_proc: process
	begin		
		Reset <= '1';
		wait until rising_edge(Clock);
		Reset <= '0';
		wait until rising_edge(Clock);
		
		F<="11110000"; Data<="ZZZZZZZZ"; -- CLRA
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="01001000"; Data<="00000000"; -- LD R0 w/ 0
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="10101000"; Data<="XXXXXXXX"; -- LD A w/ R0
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);

		F<="01000100"; Data<="01010101"; -- LD R1 with'55'
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="10000100"; Data<="XXXXXXXX"; -- Add R1 to Acc
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="10010100"; Data<="XXXXXXXX"; -- Sub R1 to Acc
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="10110100"; Data<="XXXXXXXX"; -- XCHG R1 and Acc
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="01000010"; Data<="01111111"; -- LD R2 with '7F'
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="10000010"; Data<="ZZZZZZZZ"; -- ADDA R2
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
	
		F<="11110000"; Data<="ZZZZZZZZ"; -- CLRA
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
	
		F <= "11110010"; Data <= "ZZZZZZZZ"; -- INCA
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F <= "11110010"; Data <= "ZZZZZZZZ"; -- INCA
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F <= "11110100"; Data <= "ZZZZZZZZ"; -- ~ACC
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F <= "11110011"; Data <= "ZZZZZZZZ"; -- ~ACC
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		F<="00000000"; Data<="ZZZZZZZZ"; -- NOP
		w <= '1';
		wait until rising_edge(Clock);
		w <= '0';
		wait until Done = '1' and rising_edge(Clock);
		
		
		
		wait;

--		F<="00"; Rx<="10"; Ry<="00"; Data<="00100010"; -- LD R2 with '22'
--		w <= '1';
--		wait until rising_edge(Clock);
--		w <= '0';
--		wait until Done = '1';
--		wait until rising_edge(Clock);
--
--		F<="10"; Rx<="01"; Ry<="00"; Data<="00000000"; -- ADD R1,R0
--		w <= '1';
--		wait until rising_edge(Clock);
--		w <= '0';   
--		wait until Done = '1';                  --  Observe the '7f' !
--		wait until rising_edge(Clock);
--
--		F<="01"; Rx<="11"; Ry<="01";            --  MOV  R3, R1
--		w <= '1';
--		wait until rising_edge(Clock);
--		w <= '0'; 
--		wait until Done = '1';
--		wait until rising_edge(Clock);
--
--		F<="11"; Rx<="11"; Ry<="10";            -- SUB R3,R2
--		w <= '1';
--		wait until rising_edge(Clock);
--		w <= '0';
--		wait until Done = '1';                  -- observe the 5D !
--		wait until rising_edge(Clock);
--		wait;
	end process;  
END;