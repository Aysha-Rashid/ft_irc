
//

#include "Ft_Irc.hpp"

//change name to commands
//all static functions so that they can be called without a object instentiation
class	Operator /*: public Client*/
{
	public:
	Operator();
	Operator(Operator& other);
	Operator& operator=(Operator& rhs) const;
	~Operator();

	static void kick(/*Client& client*/);
	static void invite(/**/);
	static void topic(/**/);
	static void mode(/**/);
};