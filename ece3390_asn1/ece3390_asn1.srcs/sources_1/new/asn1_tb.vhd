----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/21/2016 10:45:03 AM
-- Design Name: 
-- Module Name:  - 
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library ieee;
use ieee.STD_LOGIC_1164.all;

library work;

-- entity declaration for your testbench.Dont declare any ports here
entity asn1_tb is
end asn1_tb;

architecture behavior of asn1_tb is
    -- Component Declaration for the Unit Under Test (UUT)
    component prob1  
    port ( 
       a, b: in std_logic;
       f : out std_logic
      );
    end component;
    
    component prob2  
    port ( 
       a, b, c: in std_logic;
       f : out std_logic
      );
    end component;
       
    component prob3  
    port ( 
       a, b: in std_logic;
       f : out std_logic
      );
    end component;
    
    signal input: std_logic_vector(2 downto 0) := (others => '0');
    signal a, b, c: std_logic := '0';
    signal f1, f2, f3: std_logic;
    
begin

    c <= input(2);
    b <= input(1);
    a <= input(0);

    -- Instantiate the Unit Under Test (UUT)
    uut1: prob1 port map (
        a => a, b => b,
        f => f1
    );      

    uut2: prob2 port map (
        a => a, b => b, c => c,
        f => f2
    );     
    
    uut3: prob3 port map (
        a => a, b => b,
        f => f3
    );     

   -- Stimulus process
   stim_proc: process
   begin      
        input <= "000";  
        wait for 10 ns;
        
        input <= "001";
        wait for 10 ns;
        input <= "010";
        wait for 10 ns;
        input <= "011";
        wait for 10 ns;
        input <= "100";
        wait for 10 ns;
        input <= "101";
        wait for 10 ns;
        input <= "110";
        wait for 10 ns;
        input <= "111";
        wait for 10 ns;
        input <= "000";
        
        wait;
  end process;

end behavior;
