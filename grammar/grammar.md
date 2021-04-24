
Program ->  Header Declarations Block

Header  ->  **program** **id(input,output);**

Declarations    ->  **VAR** VariableDeclarations 
                    | **<epsilon>**

VariableDeclarations    ->  VariableDeclarations VariableDeclaratio
                            | VariableDeclaration

VariableDeclaration ->  IdentifierList **:** Type **;**

IdentifierList  ->  IdentifierList **,** **id** 
                    | **id**

Type    ->  **integer** 
            | **real** 
            | **char** 
            | **boolean**

Block   ->  **{** Statements **}**

Statements  ->  Statements **;** Statement 
                | Statement

Statement   ->  **id** **:=** Expression
                | Block
                | **if** Expression **then** Statement ElseClause
                | **for** **id** **:=** **num** **to**
                | **downto** **num** **do** Statement
                | **<epsilon>**

ElseClause  ->  **else** Statement 
                | **<epsilon>**

ExpressionList  ->  ExpressionList , Expression 
                    | Expression

Expression  ->  SimpleExpression **relop** SimpleExpression
                | SimpleExpression

SimpleExpression    ->  Term
                        | **addop** Term
                        | SimpleExpression **addop** Term

Term    ->  Term **mulop** Factor
            | Factor

Factor  ->  **id** 
            | **num** 
            | **(** Expression **)** 
            | **not** Factor 