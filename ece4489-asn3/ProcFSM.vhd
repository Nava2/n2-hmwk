LIBRARY ieee ;
USE ieee.std_logic_1164.all ;
USE ieee.std_logic_unsigned.all ;
USE work.subccts.all;
ENTITY ProcFSM IS
PORT ( Clock, Reset, w, cin: IN STD_LOGIC;
	func: in std_logic_vector(7 downto 0) ;
	Done: INOUT STD_LOGIC;
	Extern,Ain, Aout, tempIn,tempOut,AddSub, cout: OUT STD_LOGIC;
	Rin,Rout: OUT STD_LOGIC_VECTOR(0 TO 3) );
END ProcFSM;

ARCHITECTURE Behavior OF ProcFSM IS 
	SIGNAL Clear, FRin : STD_LOGIC ;
	SIGNAL Count, T, I : STD_LOGIC_VECTOR(1 DOWNTO 0) ;
	SIGNAL FuncReg : STD_LOGIC_VECTOR(7 downto 0) ;
BEGIN
	instructionReg: regn GENERIC MAP ( N => 8 ) 
		PORT MAP ( func, FRin, Clock, FuncReg ) ;

	counter: upcount PORT MAP ( Clear, Clock, Count ) ;
	T <= Count ;
	Clear <= Reset OR Done OR NOT (w OR T(1) OR T(0)) ;
	FRin <= w AND NOT (T(1) OR T(0)) ;
	
	controlSignals: process (FuncReg, T) 
		variable highF, reg : std_logic_vector(3 downto 0);
	begin
		highF := FuncReg(7 downto 4);
		reg := FuncReg(3 downto 0);
		
		Extern <= '0' ; Done <= '0' ; Ain <= '0' ; tempIn <= '0' ;
		tempOut <= '0' ; AddSub <= '0' ; Rin <= "0000" ; Rout <= "0000" ;
		cout <= '0'; Aout <= '0';
		
		case highF is
			when "1000"|"1001" => -- add RXX to ACC
				case T is
					when "00" =>
					when "01" =>
						tempIn <= '1';
						Rout <= reg;
						AddSub <= highF(0); -- tmp now has ACC + Temp
						cout <= '1';
					when others => 
						tempOut <= '1'; -- load ACC <= temp
						Ain <= '1'; Done <= '1';
				end case; -- T
				
			when "1010" => -- LD A w/ RXXX
				case T is
					when "00" =>
					when others =>
						ain <= '1';
						Rout <= reg;
						Done <= '1';
				end case;
				
			when "1011" => -- Exchange RXXX and ACC
				case T is
					when "00" =>
					when "01" =>
						Rout <= reg; -- reg => temp
						tempIn <= '1';
					when "10" =>
						Rin <= reg; -- A => temp
						Aout <= '1';
					when others =>
						Ain <= '1'; -- temp => A
						tempOut <= '1';
						Done <= '1';
						
				end case;
			
			when "0100" => -- Load RXXXX
				case T is
					when "00" =>
					when others =>
						Extern <= '1'; Rin <= FuncReg(3 downto 0); Done <= '1';
						
				end case; -- T
				
			when "0001" => -- Load Acc
				case T is
					when "00" =>
					when others =>
						Extern <= '1'; Ain <= '1'; Done <= '1';
				end case; -- T
				
		   when "0010" => -- Load Acc
				case T is
					when "00" =>
					when others =>
						Extern <= '1'; tempIn <= '1'; Done <= '1';
				end case; -- T
				
			when others => -- not implemented				
		
		end case;
	end process;
	
--	controlsignals: PROCESS ( T, I, X, Y )
--	BEGIN
--		Extern <= '0' ; Done <= '0' ; Ain <= '0' ; tempIn <= '0' ;
--		tempOut <= '0' ; AddSub <= '0' ; Rin <= "0000" ; Rout <= "0000" ;
--		CASE T IS
--			WHEN "00" => -- no signals asserted in time step T0
--			WHEN "01" => -- define signals asserted in time step T1
--				CASE I IS
--					WHEN "00" => -- Load
--						Extern <= '1' ; Rin <= X ; Done <= '1' ;
--					WHEN "01" => -- Move
--						Rout <= Y ; Rin <= X ; Done <= '1' ;
--					WHEN OTHERS => -- Add, Sub
--						Rout <= X ; Ain <= '1' ;
--				END CASE ;
--
--			WHEN "10" => -- define signals asserted in time step T2
--				CASE I IS
--					WHEN "10" => -- Add
--						Rout <= Y ; tempIn <= '1' ;
--					WHEN "11" => -- Sub
--						Rout <= Y ; AddSub <= '1' ; tempIn <= '1' ;
--					WHEN OTHERS =>	-- Load, Move
--				END CASE ;
--			WHEN OTHERS => -- define signals asserted in time step T3
--				CASE I IS
--					WHEN "00" => -- Load
--					WHEN "01" => -- Move
--					WHEN OTHERS => -- Add, Sub
--						tempOut <= '1' ; Rin <= X ; Done <= '1' ;
--				END CASE ;
--		END CASE ;
--   END PROCESS ;
END Behavior ;