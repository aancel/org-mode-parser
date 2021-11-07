#include <string>
#include <regex>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>

namespace org
{

struct Node
{
    int level;
    std::string todo;
    std::string priority;
    std::string heading;
    std::string body;

    std::vector<org::Node> children;
};

class File
{
public:
    File(std::string filename);

    void parse_file(std::string filename);
    void parse_string(std::string str);
    // will consume the vector
    static int add_subtree(std::vector<std::string> & str, ::org::Node & root);
    static void subtree_to_string(org::Node & root, std::string & str);
    std::string to_string();
    void get_flat_nodes(std::vector<Node *> & nodes);
    void add_children(Node * root, std::vector<Node *> & nodes);

    static std::string format(std::string value)
    {
        // Pango markup doesn't like & characters
        value = std::regex_replace(value, std::regex("(&)"), "&amp;");
        value = std::regex_replace(value, std::regex("(<)"), "&lt;");
        value = std::regex_replace(value, std::regex("(>)"), "&gt;");

        // From https://github.com/orgzly/orgzly-android/blob/e0918680601244b95abb684ecafff5a3ff63fbe6/app/src/main/java/com/orgzly/android/util/OrgFormatter.kt
        // Substitute org links for pango markup
        std::string s = value;
        std::smatch m;
        std::regex LINK_REGEX("(https?:\\S+)|(\\[\\[(.+?)](?:\\[(.+?)])?])");

        // Safeguard to prevent duplicated the string a the very end of the processing
        bool has_matched = false;

        // Lambda function processing the matches
        auto process_match = [](std::string & s, std::smatch & m, std::string & value)
            {
                // Get the position of the leading star and substring the rest
                if(m.length(1) != 0 )
                {
                    std::stringstream url;
                    url << "<a href=\"" << m[1] << "\">" << m[1] << "</a>";
                    value += url.str();
                    // std::cout << url.str() << std::endl;
                }
                else if(m.length(4) != 0 )
                {
                    std::stringstream url;
                    url << "<a href=\"" << m[3] << "\">" << m[4] << "</a>";
                    value += url.str();
                    // std::cout << url.str() << std::endl;
                }
                else if(m.length(2) != 0 )
                {
                    std::stringstream url;
                    url << "<a href=\"" << m[3] << "\">" << m[3] << "</a>";
                    value += url.str();
                    // std::cout << url.str() << std::endl;
                }
                s = m.suffix().str();
            };

        if(std::regex_search(s, m, LINK_REGEX))
        {
            has_matched = true;
            value = m.prefix().str();
            process_match(s, m, value);
        }
        while (std::regex_search(s, m, LINK_REGEX)) {
            has_matched = true;
            process_match(s, m, value);
        }
        // We only append the rest of the unmatched string
        // if we had at least one match
        if(has_matched)
        {
            value += s;
        }

        // str = re.sub(LINK_REGEX,
        //             dash,
        //             str)
        // # print(str)

        return value;
    }

private:
    Node m_root;
};

}
