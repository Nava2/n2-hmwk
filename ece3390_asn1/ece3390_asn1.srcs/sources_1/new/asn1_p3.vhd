----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/21/2016 11:41:52 AM
-- Design Name: 
-- Module Name: prob3 - Behavioral
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


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity prob3 is 	  
    port ( 
        a, b: in std_logic;
        f : out std_logic
       );
      
end prob3;

architecture Behavioral of prob3 is 
    signal fp: std_logic := '0';
begin  
    fp <= (b nor fp) nor a;
    f <= fp;
end Behavioral;

