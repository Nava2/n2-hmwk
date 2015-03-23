LIBRARY ieee ;
USE ieee.std_logic_1164.all ;
USE ieee.numeric_std.all ;
USE work.subccts.all ;
ENTITY Proc IS
	PORT (	Data 		: IN 		STD_LOGIC_VECTOR(7 DOWNTO 0) ;
			Reset, w 	: IN 		STD_LOGIC ;
			Clock 		: IN 		STD_LOGIC ;
			func			: IN 		std_logic_vector(7 downto 0) ;
			Done 			: INOUT 	STD_LOGIC ;
			BusWires 	: INOUT 	STD_LOGIC_VECTOR(7 DOWNTO 0) ) ;
END proc ;

ARCHITECTURE Behavior2 OF Proc IS
	SIGNAL X, Y, Rin, Rout : STD_LOGIC_VECTOR(0 TO 3) ;
	SIGNAL Clear : STD_LOGIC ;
	SIGNAL Extern, clrOut, cset, Ain, aOut, tempIn, tempOut, FRin : STD_LOGIC ;
	SIGNAL R0, R1, R2, R3 : STD_LOGIC_VECTOR(7 DOWNTO 0) ;
	SIGNAL A, Sum, temp : STD_LOGIC_VECTOR(7 DOWNTO 0) ;
	SIGNAL Sel : STD_LOGIC_VECTOR(1 TO 8) ;
	
	signal cVec, coutVec: std_logic_vector(0 to 0);
	signal carry, cin, cout: std_logic;
	signal ALUOp: std_logic_vector(3 downto 0);
	
BEGIN	
	reg0: regn PORT MAP ( BusWires, Rin(0), Clock, R0 ) ;
	reg1: regn PORT MAP ( BusWires, Rin(1), Clock, R1 ) ;
	reg2: regn PORT MAP ( BusWires, Rin(2), Clock, R2 ) ;
	reg3: regn PORT MAP ( BusWires, Rin(3), Clock, R3 ) ;
	regA: regn PORT MAP ( BusWires, Ain, Clock, A ) ;
	regCarry: regn generic map ( N => 1) port map (cVec, cin, Clock, coutVec);
	
	cVec(0) <= carry;
	cout <= coutVec(0);
	
	regTemp: regn PORT MAP ( Sum, tempIn, Clock, temp ) ; 
	Sel <= Rout & tempOut & aOut & clrOut & Extern ;
	WITH Sel SELECT
		BusWires <= 
					R0 			WHEN "10000000",
					R1 			WHEN "01000000",
					R2 			WHEN "00100000",
					R3 			WHEN "00010000",
					temp			WHEN "00001000",
					A				WHEN "00000100",
					"00000000" 	WHEN "00000010",
					Data 	WHEN OTHERS ;
					
	ControlCircuit: ProcFSM PORT MAP(Clock, Reset, w, func,
			Done, Extern, clrOut, cset, Ain, aOut, tempIn, tempOut, cin, ALUOp, Rin, Rout);
		
	alu: process(ALUOp, A, BusWires, coutVec, cset, clrOut, cout) 
		variable add, sub: signed(8 downto 0);
		variable busVal: signed(8 downto 0);
		variable AddSub: std_logic;
	begin
		carry <= 'Z';
		sum <= (others => 'Z');
	
		case ALUOp is 
			when "1111" => -- not
				sum <= not A;
			when "1110" => -- not carry
				carry <= not cout;
				
			when "0101" =>
				-- shl 1
				carry <= A(7);
				sum <= A(6 downto 0) & A(0);
				
			when "0110" =>
				-- shr 1
				carry <= A(0);
				sum <= A(7) & A(7 downto 1);
				
			when "0111" =>
				carry <= '0';
				sum <= ( 0 => cout, others => '0' );
				
			when "1001" =>
				sum(7 downto 2) <= "000010";
				carry <= '0';
				if (cout = '0') then
					sum(1 downto 0) <= "01";
				else 
					sum(1 downto 0) <= "10";
				end if;
				
			when "1011" =>
				if ((signed(A) > 9) or (cout = '1')) then
					add := signed(A(A'high) & A) + 6;
					sum <= std_logic_vector(add(add'high - 1 downto 0));
					carry <= std_logic(add(add'high - 1));
				else 
					sum <= A;
					carry <= cout;
				end if;
				
				
			when "0000"|"0010"|"0001"|"0011" =>
				
				if (ALUOp(1) = '1') then 
					busVal := "000000001";
				else 
					busVal := signed(BusWires(BusWires'high) & BusWires);
				end if;
				
				add := signed(A(A'high) & A) + busVal;
				sub := signed(A(A'high) & A) - busVal;
				addSub := ALUOp(0);
			
				case addSub is
					when '0' =>
						Sum <= std_logic_vector(add(add'high-1 downto 0));
						carry <= std_logic(add(add'high-1));
					WHEN others =>
						Sum <= std_logic_vector(sub(sub'high-1 downto 0));
						carry <= std_logic(sub(sub'high-1));
				end case;
				
			when others => -- NO-OP
		end case;
		
		if (cset = '1') then 
			carry <= '1';
		elsif (clrOut = '1') then
			carry <= '0';
		end if;
	end process;

END Behavior2 ;