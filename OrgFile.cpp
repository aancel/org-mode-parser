#include "OrgFile.hpp"
#include <vector>

namespace org
{

// From: https://github.com/karlicoss/orgparse/blob/master/orgparse/node.py
// FIXME Shouldn't be using \n here as we don't capture the first heading
static std::regex RE_NODE_HEADER("[\\n](\\*+)");
// See: https://stackoverflow.com/questions/33718410/how-to-match-line-break-in-c-regex/33719054
// For the part handling the body of the header
// FIXME flaw with the TODO keyword, works with \s*...\s+, but not \s+...\s
// static std::regex RE_HEADING_STARS("^(\\*+)\\s*([A-Z]*)\\s+(.*?)\\s*[\\n]([\\S\\s]*)");
// static std::regex RE_HEADING_STARS("^(\\*+)(\\s+([A-Z]+)\\s+)*\\s*(.*?)\\s*[\\n]([\\S\\s]*)");
static std::regex RE_HEADING_STARS("^(\\*+)(\\s+([A-Z]+)\\s+)*\\s*(.*)\\s*([\\S\\s]*)");

static std::regex RE_HEADING_TAGS("(.*?)\\s*:([\\w@:]+):\\s*$");
static std::regex RE_HEADING_PRIORITY("^\\s*\\[#([A-Z0-9])\\] ?(.*)$");
static std::regex RE_PROP("^\\s*:(.*?):\\s*(.*?)\\s*$");

File::File(std::string filename)
{
    this->parse_file(filename);
}

void File::parse_file(std::string filename)
{
    std::ostringstream sstr;
    std::ifstream in(filename, std::ios::binary);
    sstr << in.rdbuf();
    in.close();

    std::string str = sstr.str();
    this->parse_string(str);
}

void File::parse_string(std::string str)
{
    m_root.children.clear();

    std::vector<std::string> headings;
    std::sregex_iterator words_begin(str.begin(), str.end(), org::RE_NODE_HEADER);

    auto words_end = std::sregex_iterator();
    int prev_pos = -1;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;

        // Get the position of the leading star and substring the rest
        if(match.size() >= 2 )
        {
            if(prev_pos < 0)
            {
                prev_pos = match.position(1);
            }
            else
            {
                int current_pos = match.position(1);
                headings.push_back(str.substr(prev_pos, current_pos - prev_pos - 1));
                prev_pos = current_pos;
            }
        }
    }

    m_root.level = 0;
    org::File::add_subtree(headings, m_root);
}

int File::add_subtree(std::vector<std::string> & headings, ::org::Node & root)
{
    // std::cout << "INDEX " << std::to_string(headings.size()) << std::endl;
    while(headings.size() > 0)
    {
        // std::cout << "index " << std::to_string(headings.size()) << std::endl;
        std::sregex_iterator words_begin(headings.at(0).begin(), headings.at(0).end(), org::RE_HEADING_STARS);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;

            // std::cout << "index " << headings.at(0) << std::endl;
            // Get the position of the leading star and substring the rest
            if(match.size() >= 6 )
            {
                Node n;
                n.level = match[1].str().size();
                n.todo = match[3].str();
                n.heading = match[4].str();
                n.body = match[5].str();
                // std::cout << std::to_string(n.level) << std::endl;

                if(root.level < n.level)
                {
                    root.children.push_back(n);
                    headings.erase(headings.begin());
                    org::File::add_subtree(headings, root.children.back());
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                std::cout << "No match for " << headings.at(0) << std::endl;
                headings.erase(headings.begin());
            }
        }

        if(words_begin == words_end)
        {
            std::cout << "No match for " << headings.at(0) << std::endl;
            headings.erase(headings.begin());
        }
    }
    return 0;
}

void File::subtree_to_string(org::Node & root, std::string &str)
{
    str = str + "HEADING" + "\n";
    str = str + std::to_string(root.level) + "\n";
    str = str + root.heading + "\n";
    str = str + root.body + "\n";

    str = str + "# Children: " + std::to_string(root.children.size()) + "\n";
    for( auto & n : root.children )
    {
        org::File::subtree_to_string(n, str);
    }
}

void File::get_flat_nodes(std::vector<Node *> & nodes)
{
    for(int i = 0; i < m_root.children.size(); i++)
    {
        add_children(&m_root.children.at(i), nodes);
    }
}

void File::add_children(Node * root, std::vector<Node *> & nodes)
{
    nodes.push_back(root);
    for(int i = 0; i < root->children.size(); i++)
    {
        add_children(&root->children.at(i), nodes);
    }
}

std::string File::to_string()
{
    std::string str = "";

    // FIXME wrong size
    std::cout << "# Nodes: " << std::to_string(m_root.children.size()) << std::endl;
    org::File::subtree_to_string(m_root, str);

    // std::cout << str << std::endl;

    return str;
}

}
