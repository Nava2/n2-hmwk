LIBRARY ieee ;
USE ieee.std_logic_1164.all ;
USE ieee.std_logic_unsigned.all ;
USE work.subccts.all;
ENTITY ProcFSM IS
PORT ( Clock, Reset, w, cin: IN STD_LOGIC;
	func: in std_logic_vector(7 downto 0) ;
	Done: INOUT STD_LOGIC;
	Extern, clrOut, cset, Ain, Aout, tempIn,tempOut, cout: OUT STD_LOGIC;
	ALUOp: out std_logic_vector(2 downto 0);
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
		variable highF, lowF : std_logic_vector(3 downto 0);
	begin
		highF := FuncReg(7 downto 4);
		lowF := FuncReg(3 downto 0);
		
		Extern <= '0' ; Done <= '0' ; Ain <= '0' ; tempIn <= '0' ;
		tempOut <= '0' ; ALUOp <= "000"; Rin <= "0000" ; Rout <= "0000" ;
		cout <= '0'; Aout <= '0'; clrOut <= '0'; cset <= '0';
		
		case highF is
			when "1000"|"1001" => -- add RXX to ACC
				case T is
					when "00" =>
					when "01" =>
						tempIn <= '1';
						Rout <= lowF;
						ALUOp <= "00" & highF(0); -- tmp now has ACC + Temp
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
						Rout <= lowF;
						Done <= '1';
				end case;
				
			when "1011" => -- Exchange RXXX and ACC
				case T is
					when "00" =>
					when "01" =>
						Rout <= lowF; -- reg => temp
						tempIn <= '1';
					when "10" =>
						Rin <= lowF; -- A => temp
						Aout <= '1';
					when others =>
						Ain <= '1'; -- temp => A
						tempOut <= '1';
						Done <= '1';
						
				end case;
				
			when "1111" => 
				case lowF is
					when "0000" => -- Clear Acc and Carry
						case T is
							when "00" =>
							when others =>
								Ain <= '1';
								cout <= '1';
								clrOut <= '1';
								Done <= '1';
						end case;
						
					when "0001" => -- Clear Carry
						case T is
							when "00" =>
							when others =>
								cout <= '1';
								clrOut <= '1';
								Done <= '1';
						end case;
						
					when "0010" =>
						case T is
							when "00" =>
							when "01" => 
								tempIn <= '1';
								ALUOp <= "010";  -- temp <= A + 1
							when others =>
								tempOut <= '1'; -- A <= temp
								Ain <= '1'; 
								Done <= '1';
						end case;
						
					when "0011" =>
						case T is 
							when "00" =>
							when "01" =>
								tempIn <= '1'; -- temp <= ~A
								ALUOp <= "100";
							when others =>
								tempOut <= '1'; -- A <= temp
								Ain <= '1';
								Done <= '1';
							end case;
							
					when "0100" =>
						case T is 
							when "00" =>
							when others =>
								ALUOp <= "101";
								cout <= '1';
								Done <= '1';
						end case;
					
					when others => -- NOOP
						case T is
							when "00" =>
							when others => Done <= '1';
						end case;
						
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
				case T is 
					when "00" =>
					when others =>	Done <= '1';
				end case;
		end case;
	end process;

END Behavior ;