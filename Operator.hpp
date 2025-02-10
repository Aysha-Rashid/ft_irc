
#include "Ft_Irc.hpp"

class	Operator /*: public Client*/
{
	public:
	Operator();
	Operator(Operator& other);
	Operator& operator=(Operator& rhs) const;
	~Operator();

	void kick(/*Client& client*/);
	void invite(/**/);
	void topic(/**/);
	void mode(/**/);
};