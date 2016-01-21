----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01/21/2016 11:41:52 AM
-- Design Name: 
-- Module Name: prob2 - Behavioral
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


entity prob2 is 	  
    port ( 
        a, b, c: in std_logic;
        f : out std_logic
       );
      
end prob2;

architecture Behavioral2 of prob2 is 
begin  
    f <= (b nor c) nor a;
end Behavioral2;
