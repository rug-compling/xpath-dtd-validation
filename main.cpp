#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <map>
#include <set>

#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/xqilla-simple.hpp>
#include <xqilla/ast/ASTNode.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/axis/NodeTest.hpp>
#include <xercesc/util/XMLString.hpp>

#include "SimpleDTD.hh"
#include "textfile.hh"

using namespace std;

using namespace XERCES_CPP_NAMESPACE;

namespace {
    struct Globals {
        Globals();
        virtual ~Globals();
    };

    static Globals s_globals;
    
    Globals::Globals() {
        XQillaPlatformUtils::initialize();
    }
    
    Globals::~Globals() {
        XQillaPlatformUtils::terminate();
    }

    static XQilla s_xqilla;
}

class Scope
{
public:
    Scope(Scope const *parent = 0)
    :
        d_parent(parent)
    {
        //
    }

    void setNodeName(std::string const &name)
    {
        d_nodeName = name;
    }

    std::string const &nodeName() const
    {
        return d_nodeName;
    }

    std::string path() const
    {
        stringstream ss;

        Scope const *scope = this;

        while (scope)
        {
            ss << ">" << scope->nodeName();
            scope = scope->d_parent;
        }

        return ss.str();
    }

private:
    std::string d_nodeName;
    Scope const *d_parent;
};

typedef map<string, set<string> > ElementMap;

void inspect(ASTNode *node, Scope *scope, SimpleDTD const &dtd)
{
    switch (node->getType())
    {
        case ASTNode::NAVIGATION:
        {
            cout << "Type NAVIGATION" << endl;

            XQNav *nav = reinterpret_cast<XQNav*>(node);
            XQNav::Steps steps(nav->getSteps());

            for (XQNav::Steps::const_iterator it = steps.begin(); it != steps.end(); ++it)
                inspect(it->step, scope, dtd);

            break;
        }

        case ASTNode::LITERAL:
            cout << "Type LITERAL" << endl;
            break;

        case ASTNode::NUMERIC_LITERAL:
            cout << "Type NUMERIC_LITERAL" << endl;
            break;

        case ASTNode::QNAME_LITERAL:
            cout << "Type QNAME_LITERAL" << endl;
            break;

        case ASTNode::SEQUENCE:
            cout << "Type SEQUENCE" << endl;
            break;

        case ASTNode::FUNCTION:
        {
            cout << "Type FUNCTION" << endl;

            XQFunction *fun = reinterpret_cast<XQFunction *>(node);

            VectorOfASTNodes const &args(fun->getArguments());

            for (VectorOfASTNodes::const_iterator it = args.begin();
                it != args.end(); ++it)
                inspect(*it, scope, dtd);

            break;
        }

        case ASTNode::VARIABLE:
            cout << "Type VARIABLE" << endl;
            break;

        case ASTNode::STEP:
        {
            XQStep *step = reinterpret_cast<XQStep*>(node);
            NodeTest *test = step->getNodeTest();

            // Wild cards have no element name.
            if (test->getNameWildcard())
            {
                scope->setNodeName("*");
                return;
            }

            char *nodeType = XMLString::transcode(test->getNodeType());
            char *nodeName = XMLString::transcode(test->getNodeName());

            if (strcmp(nodeType, "element") == 0)
            {
                // Test to see if this element is allowed here
                if (!dtd.allowElement(nodeName, scope->nodeName()))
                    cerr << "The element " << nodeName
                         << " is not allowed inside "
                         << scope->nodeName() << endl;

                scope->setNodeName(nodeName);
            }
            
            else if (strcmp(nodeType, "attribute") == 0)
            {
                if (!dtd.allowAttribute(nodeName, scope->nodeName()))
                    cerr << "The attribute " << nodeName
                         << " is not allowed inside "
                         << scope->nodeName() << endl;
            }

            delete[] nodeType;
            delete[] nodeName;

            break;
        }

        case ASTNode::IF:
            cout << "Type IF" << endl;
            break;

        case ASTNode::INSTANCE_OF:
            cout << "Type INSTANCE_OF" << endl;
            break;

        case ASTNode::CASTABLE_AS:
            cout << "Type CASTABLE_AS" << endl;
            break;

        case ASTNode::CAST_AS:
            cout << "Type CAST_AS" << endl;
            break;

        case ASTNode::TREAT_AS:
            cout << "Type TREAT_AS" << endl;
            break;

        case ASTNode::OPERATOR:
        {
            cout << "Type OPERATOR" << endl;

            XQOperator *op = reinterpret_cast<XQOperator *>(node);
            VectorOfASTNodes const &args(op->getArguments());

            for (VectorOfASTNodes::const_iterator it = args.begin();
                it != args.end(); ++it)
                inspect(*it, scope, dtd);

            break;
        }

        case ASTNode::CONTEXT_ITEM:
            cout << "Type CONTEXT_ITEM" << endl;
            break;

        case ASTNode::DOM_CONSTRUCTOR:
            cout << "Type DOM_CONSTRUCTOR" << endl;
            break;

        case ASTNode::QUANTIFIED:
            cout << "Type QUANTIFIED" << endl;
            break;

        case ASTNode::TYPESWITCH:
            cout << "Type TYPESWITCH" << endl;
            break;

        case ASTNode::VALIDATE:
            cout << "Type VALIDATE" << endl;
            break;

        case ASTNode::FUNCTION_CALL:
            cout << "Type FUNCTION_CALL" << endl;
            break;

        case ASTNode::USER_FUNCTION:
            cout << "Type USER_FUNCTION" << endl;
            break;

        case ASTNode::ORDERING_CHANGE:
            cout << "Type ORDERING_CHANGE" << endl;
            break;

        case ASTNode::XPATH1_CONVERT:
        {
            cout << "Type XPATH1_CONVERT" << endl;

            XPath1CompatConvertFunctionArg *conv =
                reinterpret_cast<XPath1CompatConvertFunctionArg *>(node);

            inspect(conv->getExpression(), scope, dtd);

            break;
        }

        case ASTNode::PROMOTE_UNTYPED:
            cout << "Type PROMOTE_UNTYPED" << endl;
            break;

        case ASTNode::PROMOTE_NUMERIC:
            cout << "Type PROMOTE_NUMERIC" << endl;
            break;

        case ASTNode::PROMOTE_ANY_URI:
            cout << "Type PROMOTE_ANY_URI" << endl;
            break;

        case ASTNode::DOCUMENT_ORDER:
        {
            cout << "Type DOCUMENT_ORDER" << endl;

            XQDocumentOrder *docOrder = reinterpret_cast<XQDocumentOrder*>(node);
            inspect(docOrder->getExpression(), scope, dtd);

            break;
        }

        case ASTNode::PREDICATE:
        {
            cout << "Type PREDICATE" << endl;
         
            XQPredicate *predicate = reinterpret_cast<XQPredicate*>(node);
            Scope *stepScope = new Scope(scope);
            inspect(predicate->getExpression(), stepScope, dtd);
            inspect(predicate->getPredicate(), stepScope, dtd);
            break;
        }

        case ASTNode::ATOMIZE:
        {
            cout << "Type ATOMIZE" << endl;

            XQAtomize *atomize = reinterpret_cast<XQAtomize*>(node);
            inspect(atomize->getExpression(), scope, dtd);

            break;
        }

        case ASTNode::EBV:
            cout << "Type EBV" << endl;
            break;

        case ASTNode::FTCONTAINS:
            cout << "Type FTCONTAINS" << endl;
            break;

        case ASTNode::UDELETE:
            cout << "Type UDELETE" << endl;
            break;

        case ASTNode::URENAME:
            cout << "Type URENAME" << endl;
            break;

        case ASTNode::UREPLACE:
            cout << "Type UREPLACE" << endl;
            break;

        case ASTNode::UREPLACE_VALUE_OF:
            cout << "Type UREPLACE_VALUE_OF" << endl;
            break;

        case ASTNode::UTRANSFORM:
            cout << "Type UTRANSFORM" << endl;
            break;

        case ASTNode::UINSERT_AS_FIRST:
            cout << "Type UINSERT_AS_FIRST" << endl;
            break;

        case ASTNode::UINSERT_AS_LAST:
            cout << "Type UINSERT_AS_LAST" << endl;
            break;

        case ASTNode::UINSERT_INTO:
            cout << "Type UINSERT_INTO" << endl;
            break;

        case ASTNode::UINSERT_AFTER:
            cout << "Type UINSERT_AFTER" << endl;
            break;

        case ASTNode::UINSERT_BEFORE:
            cout << "Type UINSERT_BEFORE" << endl;
            break;

        case ASTNode::UAPPLY_UPDATES:
            cout << "Type UAPPLY_UPDATES" << endl;
            break;

        case ASTNode::NAME_EXPRESSION:
            cout << "Type NAME_EXPRESSION" << endl;
            break;

        case ASTNode::CONTENT_SEQUENCE:
            cout << "Type CONTENT_SEQUENCE" << endl;
            break;

        case ASTNode::DIRECT_NAME:
            cout << "Type DIRECT_NAME" << endl;
            break;

        case ASTNode::RETURN:
            cout << "Type RETURN" << endl;
            break;

        case ASTNode::NAMESPACE_BINDING:
            cout << "Type NAMESPACE_BINDING" << endl;
            break;

        case ASTNode::FUNCTION_CONVERSION:
            cout << "Type FUNCTION_CONVERSION" << endl;
            break;

        case ASTNode::SIMPLE_CONTENT:
            cout << "Type SIMPLE_CONTENT" << endl;
            break;

        case ASTNode::ANALYZE_STRING:
            cout << "Type ANALYZE_STRING" << endl;
            break;

        case ASTNode::CALL_TEMPLATE:
            cout << "Type CALL_TEMPLATE" << endl;
            break;

        case ASTNode::APPLY_TEMPLATES:
            cout << "Type APPLY_TEMPLATES" << endl;
            break;

        case ASTNode::INLINE_FUNCTION:
            cout << "Type INLINE_FUNCTION" << endl;
            break;

        case ASTNode::FUNCTION_REF:
            cout << "Type FUNCTION_REF" << endl;
            break;

        case ASTNode::FUNCTION_DEREF:
            cout << "Type FUNCTION_DEREF" << endl;
            break;

        case ASTNode::COPY_OF:
            cout << "Type COPY_OF" << endl;
            break;

        case ASTNode::COPY:
            cout << "Type COPY" << endl;
            break;

        case ASTNode::MAP:
            cout << "Type MAP" << endl;
            break;

        case ASTNode::DEBUG_HOOK:
            cout << "Type DEBUG_HOOK" << endl;
            break;
    }
}

int main(int argc, char** argv)
{
    string queryString(argc > 1 ? argv[1] : "//node[@rel='su' and string(@begin) = 4]");

	DynamicContext *ctx = s_xqilla.createContext(XQilla::XPATH2);
    ctx->setXPath1CompatibilityMode(true);

    XQQuery *query = s_xqilla.parse(X(queryString.c_str()), ctx);

    cout << query->getQueryPlan() << endl;

    ASTNode *root = query->getQueryBody();

    Scope *rootScope = new Scope();
    rootScope->setNodeName("[document root]");

    ElementMap elements;

    std::string dtdData = readFile("alpino_ds.dtd");
    SimpleDTD alpinoDtd(dtdData);

    inspect(root, rootScope, alpinoDtd);

    cout << "Done." << endl;

    delete rootScope;

	return 0;
}