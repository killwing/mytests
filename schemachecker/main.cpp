#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;
XERCES_CPP_NAMESPACE_USE

class StrX
{
public :
    explicit StrX(const XMLCh* const toTranscode)
    {
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        XMLString::release(&fLocalForm);
    }


    std::string localForm() const
    {
        return std::string(fLocalForm);
    }
    

private :
    char*   fLocalForm;
};

class XStr
{
public :
    explicit XStr(const char* const toTranscode)
    {
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    explicit XStr(const std::string& toTranscode)
    {
        fUnicodeForm = XMLString::transcode(toTranscode.c_str());
    }

    explicit XStr(int toTranscode)
    {
        std::ostringstream oss;
        oss << toTranscode;
        fUnicodeForm = XMLString::transcode(oss.str().c_str());
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    XMLCh*   fUnicodeForm;
};

#define Str(str) (StrX(str).localForm())
#define X(str) (XStr(str).unicodeForm())

template<typename T>
class DOMPtr
{
public:
    DOMPtr(T* t):t_(t)  {}
    ~DOMPtr() { t_->release(); }
    T* operator->() const { return t_; }
    T& operator*() { return *t_; }

private:
    DOMPtr(const DOMPtr&);
    DOMPtr& operator = (const DOMPtr&);
    T* t_;
};


class MyDOMErrorHandler : public DOMErrorHandler
{
public:
    MyDOMErrorHandler()
        : fSawErrors(false)
    {
    }

    bool handleError(const DOMError& domError)
    {
        fSawErrors = true;
        if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
            std::cerr << "\nWarning at file ";
        else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
            std::cerr << "\nError at file ";
        else
            std::cerr << "\nFatal Error at file ";

        std::cerr << Str(domError.getLocation()->getURI()).c_str()
            << ", line " << domError.getLocation()->getLineNumber()
            << ", char " << domError.getLocation()->getColumnNumber()
            << "\n  Message: " << Str(domError.getMessage()).c_str() << std::endl;
        return true;
    }

    void resetErrors()
    {
        fSawErrors = false;
    }

    bool getSawErrors()
    {
        return fSawErrors;
    }

private :
    MyDOMErrorHandler(const MyDOMErrorHandler&);
    void operator=(const MyDOMErrorHandler&);
    bool fSawErrors;
};

template<typename T>
class CommonDeleter
{
public:
    void operator() (T* p)
    {
        delete p;
        p = NULL;
    }
};


int main(int argc, char* argv[])
{
    if (argc != 3) 
    {
        cout << "format: ./xchecker xml xsd" << endl;
        return 0;
    }

    ifstream ifs(argv[1]);
    string xml((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    const char* schema = argv[2];

    XMLPlatformUtils::Initialize();
    try
    {
        // Instantiate the DOM parser.
        static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(gLS);
        DOMPtr<DOMBuilder> parser((static_cast<DOMImplementationLS*>(impl))->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0));

        parser->setFeature(XMLUni::fgDOMNamespaces, true);
        parser->setFeature(XMLUni::fgXercesSchema, true);
        parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
        parser->setFeature(XMLUni::fgDOMValidation, true); // always
        //parser->setFeature(XMLUni::fgDOMValidateIfSchema, true); // auto
        parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true);

        // And create our error handler and install it
        MyDOMErrorHandler errorHandler;
        parser->setErrorHandler(&errorHandler);

        // use external schema file if provided
        parser->loadGrammar(schema, Grammar::SchemaGrammarType, true);
        parser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);

        MemBufInputSource* mbis = new MemBufInputSource(reinterpret_cast<const XMLByte*>(xml.c_str()), xml.length(), "TESTXML"); // do not adopt the buffer
        Wrapper4InputSource is(mbis); // adopt the input source

        parser->parse(is);

        if (errorHandler.getSawErrors())
        {
            cerr << "parse failed" << endl;
            return 1;
        }
    } 
    catch (const std::exception& e)
    {
        cerr << "An std error occurred during parsing, message: " << e.what() << endl;
    }
    catch (const XMLException& e)
    {
        cerr << "An error occurred during parsing, message: " << Str(e.getMessage()) << endl;
    }
    catch (const DOMException& e)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        cerr << "DOMException code is:  " << e.code << endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
        {
             cerr << "Message is: " << Str(errText) << endl;
        }
    }
    catch (...)
    {
        cerr << "Unknown expcetion" << endl;
        return 1;
    }
    XMLPlatformUtils::Terminate();

    cout<<"parse ok"<<endl;
    return 0;
}

