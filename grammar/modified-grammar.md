
Program ->  Header Declarations Block

Header  ->  **program** **id(input,output);**

Declarations    ->  **VAR** VariableDeclarations 
                    | **<epsilon>**

VariableDeclarations    ->  VariableDeclaration VariableDeclarations'
VariableDeclarations'   -> VariableDeclaration VariableDeclarations' | **$\varepsilon$**

VariableDeclaration ->  IdentifierList **:** Type **;**

IdentifierList  -> **id** IdentifierList'
IdentifierList' -> **, id** IdentifierList' | **$\varepsilon$**

Type    ->  **integer** 
            | **real** 
            | **char** 
            | **boolean**

Block   ->  **{** Statements **}**

Statements  -> Statement Statements'
Statements' -> **;** Statement Statements' | **$\varepsilon$**

Statement   ->  **id** **:=** Expression
                | Block
                | **if** Expression **then** Statement ElseClause
                | **for** **id** **:=** **num** **to**
                | **downto** **num** **do** Statement
                | **<epsilon>**

ElseClause  ->  **else** Statement 
                | **<epsilon>**

ExpressionList  -> Expression ExpressionList'
ExpressionList' -> **,** Expression ExpressionList' | **$\varepsilon$**

Expression  ->  SimpleExpression **relop** SimpleExpression
                | SimpleExpression

SimpleExpression    -> Term SimpleExpression' 
                        | **addop** Term SimpleExpression'
SimpleExpression'   -> **addop** Term SimpleExpression' | **$\varepsilon$**

Term    -> Factor Term'
Term'   -> **mulop** Factor Term' | **$\varepsilon$**

Factor  ->  **id** 
            | **num** 
            | **(** Expression **)** 
            | **not** Factor 