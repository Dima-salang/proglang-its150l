
# command line program
import sys

class HLInt:
    error = False

    def __init__(self, args):
        self.args = args

    def run(self):
        if (len(self.args) == 1):
            print("Usage: python HLInt.py <filename>")
            sys.exit(1)
        elif (len(self.args) > 2):
            print("Usage: python HLInt.py <filename>")
            sys.exit(1)
        else:
            self.run_file(self.args[1])

    def run_file(self, filename):
        try:
            tokens = self.scan_file(filename)
            statements = self.parse(tokens)
            self.interpret(statements)
        except Exception as e:
            print(e)

    def scan_file(self, filename):
        with open(filename, "r") as f:
            self.clean_source(f.readlines())
            scanner = Scanner(filename)
            scanner.scan()
            scanner.print_tokens()
            return scanner.tokens

    def clean_source(self, source):
        # remove newlines and spaces
        cleaned_lines = []

        for line in source:
            indent = len(line) - len(line.lstrip(" \t"))
            stripped = line.strip()

            if not stripped:
                continue

            parts = stripped.split()
            cleaned_content = " ".join(parts)
            
            cleaned_line = (" " * indent) + cleaned_content
            cleaned_lines.append(cleaned_line)

        # write the new source to a file
        with open("NOSPACES.txt", "w") as f:
            f.writelines(cleaned_lines)

    def parse(self, tokens):
        parser = Parser(tokens)
        return parser.parse()

    def interpret(self, statements):
        interpreter = Interpreter()
        interpreter.interpret(statements)

    def error(self, message):
        print(message)
        self.error = True

class Scanner:
    def __init__(self, source):
        self.source = source
        self.tokens = []
        self.start_idx = 0
        self.current_idx = 0
        self.line = 1
        self.has_error = False

        # indentation
        self.indent_level = 0
        self.indent_stack = [0]
        self.is_at_start_line = True
    
    def scan_file(self, filename):
        with open(filename, "r") as f:
            self.source = f.read()
        
    def scan(self):
        self.scan_file(self.source)
        while not self.is_at_end():
            # handle the indentation problem
            if self.is_at_start_line:
                self.handle_indentation()
            
            if not self.is_at_end():
                self.start_idx = self.current_idx
                self.scan_token()
        
        # pop all remaining indentations
        while self.indent_stack[-1] > 0:
            self.indent_stack.pop()
            self.tokens.append(Token("DEDENT", "", None, self.line))

        self.add_token("EOF")

    def handle_indentation(self):
        current_indent = 0
        self.is_at_start_line = False
        while self.next_char() == " ":
            self.advance_char()
            current_indent += 1

        last_indent = self.indent_stack[-1]

        if self.next_char() == "\n":
            self.is_at_start_line = True
            return


        # we indent
        if current_indent > last_indent:
            self.indent_stack.append(current_indent)
            self.tokens.append(Token("INDENT", "", None, self.line))
        # there is a dedent that occured
        elif current_indent < last_indent:
            while self.indent_stack[-1] > current_indent:
                self.indent_stack.pop()
                self.tokens.append(Token("DEDENT", "", None, self.line))

            if self.indent_stack[-1] != current_indent:
                self.error("Indentation error")
        

    def scan_token(self):
        # since we are scanning tokens, we are not at the start of line
        self.is_at_start_line = False

        if self.next_char() == " ":
            # do nothing since it is just whitespace
            self.advance_char()
            return
            
        character = self.advance_char()
        match character:
            case "+":
                self.add_token("PLUS")
            case "-":
                self.add_token("MINUS")
            case "*":
                self.add_token("STAR")
            case "/":
                self.add_token("SLASH")
            case "!":
                self.add_token("BANG_EQUAL" if self.is_next("=") else "BANG")
            case "<":
                if self.is_next("="):
                    self.add_token("LESS_EQUAL")
                elif self.is_next("<"):
                    self.add_token("PRINT_OP")
                else:
                    self.add_token("LESS")
            case ">":
                self.add_token("GREATER_EQUAL" if self.is_next("=") else "GREATER")
            case "=":
                self.add_token("EQUAL_EQUAL" if self.is_next("=") else "EQUAL")
            case ":":
                self.add_token("COLON_EQUAL" if self.is_next("=") else "COLON")
            case "(":
                self.add_token("LEFT_PAREN")
            case ")":
                self.add_token("RIGHT_PAREN")
            case ";":
                self.add_token("SEMICOLON")
            case "\n":
                self.line += 1
                self.add_token("NEWLINE")
                self.is_at_start_line = True
            case '"':
                self.add_string()
            case _:
                if character.isdigit():
                    self.add_number()
                elif character.isalpha():
                    self.add_identifier()
                else:
                    self.error(f"Unexpected character: {character}")

    def is_next(self, expected):
        if self.is_at_end(): return False
        if (self.source[self.current_idx] != expected): return False
        self.current_idx += 1
        return True

    
    def advance_char(self):
        self.current_idx += 1
        return self.source[self.current_idx - 1]

        

    def is_at_end(self):
        return self.current_idx >= len(self.source)

    
    def add_token(self, type, value=None):
        text = self.source[self.start_idx:self.current_idx]
        self.tokens.append(Token(type, text, value, self.line))

    def add_string(self):
        while self.next_char() != '"' and not self.is_at_end():
            if self.next_char() == '\n':
                self.line += 1
            self.advance_char()

        if self.is_at_end():
            self.error("Unterminated string")
        self.advance_char()
        unquoted = self.source[self.start_idx+1:self.current_idx-1]
        self.add_token("STRING", unquoted)

    def add_number(self):
        # scan for the entire integer part
        while self.next_char().isdigit() and not self.is_at_end():
            self.advance_char()

        # scan for the entire fractional part
        if (self.next_char() == "." and self.next_char(1).isdigit()):
            self.advance_char()
            while self.next_char().isdigit() and not self.is_at_end():
                self.advance_char()
        self.add_token("NUMBER", float(self.source[self.start_idx:self.current_idx]))

    def add_identifier(self):
        while self.next_char().isalpha() and not self.is_at_end():
            self.advance_char()
        text = self.source[self.start_idx:self.current_idx]
        if text in RESERVED_WORDS:
            self.add_token(RESERVED_WORDS[text])
        else:
            self.add_token("IDENTIFIER", text)

    def next_char(self, offset=0):
        if self.is_at_end(): return "\0"
        return self.source[self.current_idx + offset]

    def print_tokens(self):
        # write to RES_SYM.txt
        with open("RES_SYM.txt", "w") as f:
            for token in self.tokens:
                f.write(str(token) + "\n")


    def error(self, message):
        print(message)
        self.has_error = True
    
            

    


        
        
        
class Token:
    def __init__(self, type, lexeme, literal, line):
        self.type = type
        self.lexeme = lexeme
        self.literal = literal
        self.line = line

    def __str__(self):
        return f"{self.type} {self.lexeme} {self.literal} {self.line}"



TOKEN_TYPES = [
    "IDENTIFIER",
    "NUMBER",
    "PLUS",
    "MINUS",
    "EQUAL",
    "COLON",
    "EQUAL_EQUAL",
    "NOT_EQUAL",
    "GREATER",
    "GREATER_EQUAL",
    "LESS",
    "LESS_EQUAL",
    "NEWLINE",
    "INDENT",
    "DEDENT",
    "PRINT_OP"
]


RESERVED_WORDS = {
    "if": "IF",
    "output": "OUTPUT",
    "integer": "INTEGER",
    "double": "DOUBLE",
}
    

# Parser

class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.current_idx = 0
        self.has_error = False

    def parse(self):
        try:
            statements = []
            while not self.is_at_end():
                if self.match("NEWLINE"):
                    continue
                statements.append(self.declaration())
            return statements
        except Exception as e:
            print(e)

    def declaration(self):
        print("next token: ", self.next_token().type)
        if self.next_token().type == "IDENTIFIER":
            if self.next_token(1).type == "COLON":
                return self.var_decl_stmt()
        return self.statement()

    # statements
    def statement(self):
        if self.match("OUTPUT"):
            return self.output_stmt()
        return self.expression_stmt()

    def output_stmt(self):
        self.consume("PRINT_OP", "Expect '<<' after expression")
        expression = self.expression()
        self.consume("SEMICOLON", "Expect ';' after expression")
        return OutputStmt(expression)

    def expression_stmt(self):
        expression = self.expression()
        self.consume("SEMICOLON", "Expect ';' after expression")
        return ExpressionStmt(expression)

    def var_decl_stmt(self):
        name = self.consume("IDENTIFIER", "Expect variable name")

        self.consume("COLON", "Expect ':' after variable name")
        data_type = self.data_type()

        self.consume("SEMICOLON", "Expect ';' after declaration")
        return VarDeclStmt(name, data_type)

    def data_type(self):
        if self.check_type("INTEGER"):
            token = self.next_token()
            self.consume("INTEGER", "Expect 'integer' after ':'")
            return token
        if self.check_type("DOUBLE"):
            token = self.next_token()
            self.consume("DOUBLE", "Expect 'double' after ':'")
            return token
        self.error(self.next_token(), "Expect data type")
    

    # define the nonterminals 
    def expression(self):
        return self.assignment()

    def assignment(self):
        expr = self.equality()
        if self.match("COLON_EQUAL"):
            equals = self.previous_token()
            value = self.assignment()
            if (isinstance(expr, Variable)):
                assignment = Assign(expr.name, value)
                return assignment
            self.error(equals, "Invalid assignment target")
        return expr

    def equality(self):
        expr = self.comparison()
        while self.match("EQUAL_EQUAL", "BANG_EQUAL"):
            operator = self.previous_token()
            right = self.comparison()
            expr = Binary(expr, operator, right)
        return expr

    def comparison(self):
        expr = self.term()
        while self.match("GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL"):
            operator = self.previous_token()
            right = self.term()
            expr = Binary(expr, operator, right)
        return expr

    def term(self):
        expr = self.factor()
        while self.match("PLUS", "MINUS"):
            operator = self.previous_token()
            right = self.factor()
            expr = Binary(expr, operator, right)
        return expr

    def factor(self):
        expr = self.unary()
        while self.match("STAR", "SLASH"):
            operator = self.previous_token()
            right = self.unary()
            expr = Binary(expr, operator, right)
        return expr

    def unary(self):
        if self.match("BANG", "MINUS"):
            operator = self.previous_token()
            right = self.unary()
            return Unary(operator, right)
        return self.primary()

    def primary(self):
        if self.match("FALSE"): return Literal(False)
        if self.match("TRUE"): return Literal(True)
        if self.match("NONE"): return Literal(None)
        if self.match("NUMBER", "STRING"):
            literal = Literal(self.previous_token().literal)
            return literal
        if self.match("LEFT_PAREN"):
            expr = self.expression()
            self.consume("RIGHT_PAREN", "Expect right parenthesis after expression")
            grouping = Grouping(expr)
            return grouping
        if self.match("IDENTIFIER"):
            return Variable(self.previous_token().lexeme)
        self.error(self.next_token(), "Expect expression")


    def advance_token(self):
        if self.is_at_end(): return None
        self.current_idx += 1
        return self.previous_token()


    def match(self, *types):
        if self.is_at_end():
            return False
        if self.next_token().type in types:
            self.advance_token()
            return True
        return False

    def check_type(self, type):
        if self.is_at_end(): return False
        if self.next_token().type == type:
            return True
        return False
    
    def is_at_end(self):
        if self.current_idx >= len(self.tokens):
            return True
        return self.tokens[self.current_idx].type == "EOF"

    def next_token(self, offset=0):
        if self.is_at_end(): return None
        token = self.tokens[self.current_idx + offset]
        return token

    def previous_token(self, offset=1):
        if self.is_at_end(): return None
        token = self.tokens[self.current_idx - offset]
        return token

    def consume(self, type, message):
        if self.check_type(type):
            return self.advance_token()
        self.error(self.next_token(), message)

    def error(self, token, message):
        if token.type == "EOF":
            print(f"At line {token.line} at end: {message}")
        else:
            print(f"At line {token.line}: {message}")
        self.has_error = True


# interpreter class for evaluating expressions
class Interpreter:
    def __init__(self):
        self.environment = Environment()

    def interpret(self, statements):
        try:
            for statement in statements:
                self.evaluate_stmt(statement)
        except RuntimeError as e:
            print(e)

    def evaluate(self, expr):
        if isinstance(expr, Literal):
            # we just return the value of the literal token
            return expr.value
        if isinstance(expr, Grouping):
            # recursively evaluate the expression inside the grouping
            return self.evaluate(expr.expression)
        if isinstance(expr, Binary):
            # recursively evaluate the operands
            left = self.evaluate(expr.left)
            right = self.evaluate(expr.right)
            match expr.operator.type:
                case "PLUS":
                    # concatenation
                    if (isinstance(left, str) or isinstance(right, str)):
                        return str(left) + str(right)
                    # else just normal addition
                    return (float(left) + float(right))
                case "MINUS":
                    return (float(left) - float(right))
                case "STAR":
                    return (float(left) * float(right))
                case "SLASH":
                    return (float(left) / float(right))
                case "GREATER":
                    return (float(left) > float(right))
                case "GREATER_EQUAL":
                    return (float(left) >= float(right))
                case "LESS":
                    return (float(left) < float(right))
                case "LESS_EQUAL":
                    return (float(left) <= float(right))
                case "EQUAL_EQUAL":
                    return self.is_equal(left, right)
                case "BANG_EQUAL":
                    return not self.is_equal(left, right)
        if isinstance(expr, Unary):
            right = self.evaluate(expr.right)
            match expr.operator.type:
                case "BANG":
                    return not right
                case "MINUS":
                    return -(float(right))

        if isinstance(expr, Variable):
            return self.environment.get(expr.name)
        if isinstance(expr, Assign):
            value = self.evaluate(expr.value)
            self.environment.assign(expr.name, value)
            return value
        raise RuntimeError(f"Unsupported expression type: {expr.type}")

    def evaluate_stmt(self, stmt):
        if isinstance(stmt, ExpressionStmt):
            return self.evaluate(stmt.expression)
        if isinstance(stmt, OutputStmt):
            value = self.evaluate(stmt.expression)
            print(self.to_string(value))
            return value
        if isinstance(stmt, VarDeclStmt):
            print("Declaring variable: ", stmt.name.lexeme)
            self.environment.define(stmt.name.lexeme, stmt.value)
            self.environment.print_dict()
            return stmt.value
        raise RuntimeError(f"Unsupported statement type: {stmt}")

    def is_equal(self, obj_a, obj_b):
        if obj_a is None and obj_b is None:
            return True
        if obj_a is None or obj_b is None:
            return False
        return obj_a == obj_b

    
    def is_truthy(self, obj):
        if obj is None or obj == False:
            return False
        if isinstance(obj, bool):
            return obj
        return True


    def to_string(self, obj):
        if obj is None:
            return "Null"
        if isinstance(obj, float):
            return str(obj)
        return obj


class Stmt:
    """ Base class """
    pass

class ExpressionStmt(Stmt):
    def __init__(self, expression):
        self.expression = expression

class OutputStmt(Stmt):
    def __init__(self, expression):
        self.expression = expression

    def output(self):
        print(self.expression)

class VarDeclStmt(Stmt):
    def __init__(self, name, value):
        self.name = name
        self.value = value

class IfStmt(Stmt):
    def __init__(self, condition, then_branch, else_branch):
        self.condition = condition
        self.then_branch = then_branch
        self.else_branch = else_branch

class BlockStmt(Stmt):
    def __init__(self, statements):
        self.statements = statements


class DataType:
    """ Base class """
    pass

class IntegerType(DataType):
    def __init__(self):
        self.type = "INTEGER"

class DoubleType(DataType):
    def __init__(self):
        self.type = "DOUBLE"

# AST Nodes
class Expr:
    """  Base class   """
    pass


class Binary(Expr):
    def __init__(self, left, operator, right):
        self.left = left
        self.operator = operator
        self.right = right
    
    def __str__(self):
        return f"({self.left} {self.operator} {self.right})"

class Unary(Expr):
    def __init__(self, operator, right):
        self.operator = operator
        self.right = right
    
    def __str__(self):
        return f"({self.operator} {self.right})"


class Grouping(Expr):
    def __init__(self, expression):
        self.expression = expression
    
    def __str__(self):
        return f"({self.expression})"

class Literal(Expr):
    def __init__(self, value):
        self.value = value
    
    def __str__(self):
        return f"{self.value}"

class Variable(Expr):
    def __init__(self, name):
        self.name = name
    
    def __str__(self):
        return f"{self.name}"

class Assign(Expr):
    def __init__(self, name, value):
        self.name = name
        self.value = value
    
    def __str__(self):
        return f"{self.name} := {self.value}"

class Environment:
    def __init__(self):
        self.values = {}

    def get(self, token_name):
        print("Getting variable: ", token_name)
        if token_name in self.values.keys():
            print(self.values[token_name])
            return self.values[token_name]
        raise RuntimeError(f"Undefined variable {token_name}")
        

    # variable definition binding
    # uses the name of the variable as the lookup key
    def define(self, name, value):
        self.values[name] = value

    def assign(self, name, value):
        name = name.lstrip()
        print("Assigning variable: ", name)
        if name in self.values.keys():
            self.values[name] = value
            return
        raise RuntimeError(f"Undefined variable {name}")
    
    def print_dict(self):
        for key, value in self.values.items():
            print(f"{key}: {value}")


def main():
    HLInt(sys.argv).run()


if __name__ == "__main__":
    main()
