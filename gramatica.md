\begin{codeblock}
<Programa> ::= <DeclaracionGlobal>*

<DeclaracionGlobal> ::= <DeclaracionFuncion>

<DeclaracionFuncion> ::= {\color{blue}fn} <Identificador> {\color{blue}(} [<ListaParametros>] {\color{blue})} 
            [{\color{blue}->} <Tipo>] <BloqueCodigo>

<BloqueCodigo> ::= {\color{blue}\{} <Sentencia>* {\color{blue}\}}

<ListaParametros> ::= <Identificador> {\color{blue}:} [{\color{blue} &mut} ] <Tipo> 
                    ({\color{blue},} <Identificador> {\color{blue}:} [{\color{blue} &mut} ] <Tipo>)*

<Sentencia> ::= <DeclaracionVariable>
              | <Asignacion> {\color{blue};}
              | {\color{blue}println!} {\color{blue}(} <PrintExp> {\color{blue})} {\color{blue};}
              | <SentenciaControl>
              | {\color{blue}return} [<CExp>] {\color{blue};}
              | <LlamadaFuncion> {\color{blue};}
              | {\color{blue}break} {\color{blue};}


<DeclaracionVariable> ::= {\color{blue}let} [{\color{blue}mut}] <Identificador> 
                [{\color{blue}:} <Tipo> | {\color{blue}[} <Tipo> {\color{blue};} <LiteralEntero> {\color{blue}]}] [{\color{blue}=} <CExp>] {\color{blue};}

<Tipo> ::= {\color{blue}i64} |  {\color{blue}f64} | {\color{blue}bool} 

<Asignacion> ::= <Identificador> {\color{blue}=} <CExp>
               | <AccesoArray> {\color{blue}=} <CExp>

<PrintExp> ::= <LiteralCadena> [{\color{blue},} <CExp>]*

<LiteralCadena> ::= {\color{blue}"}(~{\color{blue}"})+{\color{blue}"}

<SentenciaControl> ::= <IfElseSentencia>
                     | <WhileSentencia>
                     | <ForSentencia>

<IfElseSentencia> ::= {\color{blue}if} <CExp> <BloqueCodigo> [{\color{blue}else} <BloqueCodigo>]

<WhileSentencia> ::= {\color{blue}while} <CExp> <BloqueCodigo>

<ForSentencia> ::= {\color{blue}for} <Identificador> {\color{blue}in} <Exp>{\color{blue}..}<Exp> <BloqueCodigo>
 
<CExp> ::= <LExp>
<LExp> ::= <RExp> ( ({\color{blue}&&} | {\color{blue}||}) <RExp> )*
<RExp> ::= <Exp> ( <OperadorRelacional> <Exp> )?
<Exp> ::= <Term> ( <OperadorAditivo> <Term> )*
<Term> ::= <Factor> ( <OperadorMultiplicativo> <Factor> )*
<Factor> ::= <Identificador>
           | <Literal>
           | <LiteralArray>
           | <Booleano>
           | {\color{blue}(} <CExp> {\color{blue})}
           | <LlamadaFuncion>
           | <AccesoArray>
           | <ExpresionUnaria>

<Identificador> ::= ({\color{blue}a..z} | {\color{blue}A..Z} | {\color{blue}_}) ({\color{blue}a..z} | {\color{blue}A..Z} | {\color{blue}0..9} | {\color{blue}_})+

<Literal> ::= <LiteralEntero> | <LiteralFlotante> 
<LiteralEntero> ::= ({\color{blue}0..9})+
<LiteralFlotante> ::= ({\color{blue}0..9})+ {\color{blue}.} ({\color{blue}0..9})+

<LiteralArray> ::= {\color{blue}[} <CExp> ({\color{blue},} <CExp>)* {\color{blue}]}

<Booleano> ::= {\color{blue}true} | {\color{blue}false}

<LlamadaFuncion> ::= <Identificador> {\color{blue}(} [<ListaArgumentos>] {\color{blue})}

<ListaArgumentos> ::=  [{\color{blue} &mut} ] <CExp> ({\color{blue},} [{\color{blue} &mut} ] <CExp>)*

<AccesoArray> ::= <Identificador> {\color{blue}[} <CExp> {\color{blue}]}

<OperadorRelacional> ::= {\color{blue}==} | {\color{blue}!=} | {\color{blue}<} | {\color{blue}>} | {\color{blue}<=} | {\color{blue}>=}
                        
<OperadorAditivo> ::= {\color{blue}+} | {\color{blue}-}

<OperadorMultiplicativo> ::= {\color{blue}*} | {\color{blue}/}

<ExpresionUnaria> ::= ({\color{blue}-} | {\color{blue}!}) <Factor>
\end{codeblock}