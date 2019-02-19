#include <iostream>
#include <map>
#include <iomanip>
#include <vector>
#include <string>

const std::string blue("\033[0;34m");
const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string reset("\033[0m");


    
class TextTable {

    public:
    enum class Alignment { LEFT, RIGHT }; 
    typedef std::vector< std::string > Row;
    TextTable( char horizontal = '-', char vertical = '|', char corner = '+' ) :
        _horizontal( horizontal ),
        _vertical( vertical ),
        _corner( corner )
    {}

    void setAlignment( unsigned i, Alignment alignment )
    {
        _alignment[ i ] = alignment;
    }

    Alignment alignment( unsigned i ) const
    { return _alignment[ i ]; }

    char vertical() const
    { return _vertical; }

    char horizontal() const
    { return _horizontal; }

    void add( std::string const & content ,string color = reset)
    {
        _current.push_back( content );
        _current_color.push_back( color );
    }

    void endOfRow()
    {
        _rows.push_back( _current );
        _current.assign( 0, "" );
        _rows_color.push_back(_current_color);
        _current_color.assign(0, "");
    }

    template <typename Iterator>
    void addRow( Iterator begin, Iterator end )
    {
        for( auto i = begin; i != end; ++i ) {
           add( * i ); 
        }
        endOfRow();
    }

    template <typename Container>
    void addRow( Container const & container )
    {
        addRow( container.begin(), container.end() );
    }

    std::vector< Row > const & rows() const
    {
        return _rows;
    }
    
    std::vector< Row > const & rows_color() const
    {
        return _rows_color;
    }

    void setup() const
    {
        determineWidths();
        setupAlignment();
    }

    std::string ruler() const
    {
        std::string result;
        result += _corner;
        for( auto width = _width.begin(); width != _width.end(); ++ width ) {
            result += repeat( * width, _horizontal );
            result += _corner;
        }

        return result;
    }

    int width( unsigned i ) const
    { return _width[ i ]; }

    private:
    char _horizontal;
    char _vertical;
    char _corner;
    Row _current;
    Row _current_color;

    std::vector< Row > _rows;
    std::vector< Row > _rows_color;

    std::vector< unsigned > mutable _width;
    std::map< unsigned, Alignment > mutable _alignment;

    static std::string repeat( unsigned times, char c )
    {
        std::string result;
        for( ; times > 0; -- times )
            result += c;

        return result;
    }

    unsigned columns() const
    {
        return _rows[ 0 ].size();
    }

    void determineWidths() const
    {
        _width.assign( columns(), 0 );
        for ( auto rowIterator = _rows.begin(); rowIterator != _rows.end(); ++ rowIterator ) {
            Row const & row = * rowIterator;
            for ( unsigned i = 0; i < row.size(); ++i ) {
                _width[ i ] = _width[ i ] > row[ i ].size() ? _width[ i ] : row[ i ].size();
            }
        }
    }

    void setupAlignment() const
    {
        for ( unsigned i = 0; i < columns(); ++i ) {
            if ( _alignment.find( i ) == _alignment.end() ) {
                _alignment[ i ] = Alignment::LEFT;
            }
        }
    }
};

std::ostream & operator<<( std::ostream & stream, TextTable const & table )
{
    table.setup();
    stream << table.ruler() << "\n";
    for ( auto rowIterator = table.rows().begin(), row_colorIterator = 
            table.rows_color().begin(); rowIterator != table.rows().end(); 
            ++ rowIterator, ++row_colorIterator) {
        TextTable::Row const & row = * rowIterator; 
        TextTable::Row const & row_color = * row_colorIterator; 
        stream << table.vertical();
        for ( unsigned i = 0; i < row.size(); ++i ) {
            auto alignment = table.alignment( i ) == TextTable::Alignment::LEFT ? std::left : std::right;
            stream << row_color[i] << std::setw( table.width( i ) ) << alignment  << row[ i ] << reset;
            stream << table.vertical();
        }
        stream << "\n";
        stream << table.ruler() << "\n";
    }

    return stream;
}   
