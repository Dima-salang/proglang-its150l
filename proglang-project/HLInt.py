
# command line program
import sys

# main token class
class Token:
    def __init__(self, type, lexeme, literal, line):
        self.type = type
        self.lexeme = lexeme
        self.literal = literal
        self.line = line

    def __str__(self):
        return f"{self.type} {self.lexeme} {self.literal} {self.line}"

# possible token types just for reference
TOKEN_TYPES = [
    "IDENTIFIER",
    "PLUS",
    "MINUS",
    "EQUAL",
    "COLON",
    "COLON_EQUAL",
    "EQUAL_EQUAL",
    "NOT_EQUAL",
    "GREATER",
    "GREATER_EQUAL",
    "LESS",
    "LESS_EQUAL",
    "NEWLINE",
    "INDENT",
    "DEDENT",
    "PRINT_OP",
    "EOF",
    "OUTPUT",
    "IF",
    "INTEGER",
    "DOUBLE",
]

# reserved words and corresponding token types
RESERVED_WORDS = {
    "if": "IF",
    "output": "OUTPUT",
    "integer": "INTEGER",
    "double": "DOUBLE",
}

# driver class for the interpreter
class HLInt:

    def __init__(self, args):
        self.args = args
        self.error = False

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
        except ParserError as e:
            self.error = True
            print("Error: " + str(e))
        except RuntimeError as e:
            self.error = True
            print("Error: " + str(e))
        finally:
            print("ERROR(S)!" if self.error else "NO ERROR(S)!")


    def scan_file(self, filename):
        with open(filename, "r") as f:
            self.clean_source(f.readlines())
            scanner = Scanner(filename)
            scanner.scan()
            scanner.print_tokens()
            return scanner.tokens

    def clean_source(self, source):
        # remove newlines and spaces except for the indentation
        cleaned_lines = []

        for line in source:
            indent = len(line) - len(line.lstrip(" \t"))
            stripped = line.strip()

            if not stripped:
                continue

            cleaned_content = " ".join(stripped.split())
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

# scanner class for lexical analysis
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
                raise ParserError(self.tokens[-1], "Indentation error")
        

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
                self.add_token("NEWLINE", "\\n")
                self.is_at_start_line = True
            case '"':
                self.add_string()
            case _:
                if character.isdigit():
                    self.add_number()
                elif character.isalpha():
                    self.add_identifier()
                else:
                    raise ParserError(self.tokens[-1], f"Unexpected character: {character}")

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
            raise ParserError(self.tokens[-1], "Unterminated string")
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
            f.write("RESERVED WORDS AND SYMBOLS\n\n")
            f.write("| Type | Lexeme | Line |\n")
            for token in self.tokens:
                f.write(f"{token.type} {repr(token.lexeme)} {token.line} \n")

# Parser for building the abstract syntax tree
class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.current_idx = 0
        self.has_error = False

    def parse(self):
        statements = []
        while not self.is_at_end():
            if self.match("NEWLINE"):
                continue
            statements.append(self.declaration())
        return statements

    def declaration(self):
        try:
            if self.next_token().type == "IDENTIFIER":
                if self.next_token(1).type == "COLON":
                    return self.var_decl_stmt()
            return self.statement()
        except ParserError as e:
            print("Error: " + str(e))
            self.synchronize()

    # statements
    def statement(self):
        if self.match("OUTPUT"):
            return self.output_stmt()
        if self.match("IF"):
            return self.if_stmt()
        return self.expression_stmt()

    def output_stmt(self):
        self.consume("PRINT_OP", "Expect '<<' after expression")
        expression = self.expression()
        self.consume("SEMICOLON", "Expect ';' after expression")
        return OutputStmt(expression)

    def if_stmt(self):
        self.consume("LEFT_PAREN", "Expect '(' after 'if'")
        condition = self.expression()
        self.consume("RIGHT_PAREN", "Expect ')' after 'if'")
        self.consume("NEWLINE", "Expect newline after 'if'")
        self.consume("INDENT", "Expect indent after 'if'")
        then_branch = self.block()

        else_branch = None
        if (self.check_type("ELSE")):
            self.consume("ELSE", "Expect 'else' after 'if'")
            self.consume("NEWLINE", "Expect newline after 'else'")
            self.consume("INDENT", "Expect indent after 'else'")
            else_branch = self.block()
            self.consume("DEDENT", "Expect dedent after 'else'")
        self.consume("DEDENT", "Expect dedent after 'if'")
        return IfStmt(condition, then_branch, else_branch)

    def expression_stmt(self):
        expression = self.expression()
        self.consume("SEMICOLON", "Expect ';' after expression")
        return ExpressionStmt(expression)

    def var_decl_stmt(self):
        name = self.consume("IDENTIFIER", "Expect variable name")

        self.consume("COLON", "Expect ':' after variable name")
        data_type = self.data_type()

        self.consume("SEMICOLON", "Expect ';' after declaration")
        return VarDeclStmt(name, None, data_type)

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

    def block(self):
        statements = []
        while not self.check_type("DEDENT") and not self.is_at_end():
            if self.match("NEWLINE"):
                continue
            statements.append(self.declaration())
        return BlockStmt(statements)
    

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
        raise ParserError(self.next_token(), message)

    def error(self, token, message):
        self.has_error = True
        if token and token.type == "EOF":
            err_msg = (f"At line {token.line} at end: {message}")
        elif token:
            err_msg = (f"At line {token.line}: {message}")
        else:
            err_msg = message
        raise RuntimeError(err_msg)
    
    def synchronize(self):
        self.advance_token()
        while not self.is_at_end():
            if self.previous_token().type == "SEMICOLON":
                return

            match(self.next_token().type):
                case "IF": break
                case "OUTPUT": break
                case "INTEGER": break
                case "DOUBLE": break
                case "NONE": break
                case "TRUE": break
                case "FALSE": break
                case "IDENTIFIER": break
                case "LEFT_PAREN": break
                case "COLON": break
                case "COLON_EQUAL": break
                case "PLUS": break
                case "MINUS": break
                case "STAR": break
                case "SLASH": break
                case "GREATER": break
                case "GREATER_EQUAL": break
                case "LESS": break
                case "LESS_EQUAL": break
                case "EQUAL_EQUAL": break
                case "BANG_EQUAL": break
                case "BANG": break
                case "NEWLINE": break
                case "INDENT": break
                case "DEDENT": break
                case "EOF": return

            self.advance_token()

# ParserError class for error handling
class ParserError(Exception):
    def __init__(self, token, message):
        self.line = token.line
        self.message = message
        self.token = token

    def __str__(self):
        if self.token.type == "EOF":
            return f"At line {self.line} at end: {self.message}"
        elif self.token:
            return f"At line {self.line}: {self.message}"
        return self.message

# interpreter class for evaluating expressions in the environment
class Interpreter:
    def __init__(self):
        self.environment = Environment()

    def interpret(self, statements):
        try:
            for statement in statements:
                self.evaluate_stmt(statement)
        except ParserError as e:
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
        raise ParserError(expr, f"Unsupported expression type: {expr.type} at line {expr.line}")

    def evaluate_stmt(self, stmt):
        if isinstance(stmt, ExpressionStmt):
            return self.evaluate(stmt.expression)
        if isinstance(stmt, OutputStmt):
            value = self.evaluate(stmt.expression)
            print(self.to_string(value))
            return value
        if isinstance(stmt, VarDeclStmt):
            value = None
            if stmt.value is not None:
                value = self.evaluate(stmt.value)
            self.environment.define(stmt.name.lexeme, value)
            return value
        if isinstance(stmt, IfStmt):
            if (self.is_truthy(self.evaluate(stmt.condition))):
                self.evaluate_stmt(stmt.then_branch)
            else:
                if (stmt.else_branch is not None):
                    self.evaluate_stmt(stmt.else_branch)
            return
        if isinstance(stmt, BlockStmt):
            for statement in stmt.statements:
                self.evaluate_stmt(statement)
            return
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
            return str(round(obj, 2))
        return obj

# environment where state and execution of code takes place
class Environment:
    def __init__(self):
        self.values = {}

    def get(self, token_name):
        if token_name in self.values.keys():
            return self.values[token_name]
        raise ParserError(token_name, f"Undefined variable {token_name}")
        

    # variable definition binding
    # uses the name of the variable as the lookup key
    def define(self, name, value):
        self.values[name] = value

    def assign(self, name, value):
        if name in self.values.keys():
            self.values[name] = value
            return
        raise ParserError(name, f"Undefined variable {name}")
    
    def print_dict(self):
        for key, value in self.values.items():
            print(f"{key}: {value}")

# AST Nodes
# statements
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
    def __init__(self, name, value, data_type):
        self.name = name
        self.value = value
        self.data_type = data_type

class IfStmt(Stmt):
    def __init__(self, condition, then_branch, else_branch):
        self.condition = condition
        self.then_branch = then_branch
        self.else_branch = else_branch

class BlockStmt(Stmt):
    def __init__(self, statements):
        self.statements = statements

# expressions
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



def main():
    HLInt(sys.argv).run()


if __name__ == "__main__":
    main()
