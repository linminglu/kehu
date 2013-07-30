#ifndef _FCGLOBALCONFIG_H_
#define _FCGLOBALCONFIG_H_

#define CLASS_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

#endif  // _FCGLOBALCONFIG_H_