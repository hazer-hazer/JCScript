#ifndef SCOPE_H
#define SCOPE_H

class Scope {
	public:
		Scope(Scope * parent = nullptr){}
		virtual ~Scope() = default;

		Scope * get_parent();
		
	private:
		Scope * parent;

};

#endif