#include <cstdlib>
#include <cstdint>
#include <cstring>

/*
 *  Defines the maximum size a package can have in bytes. 
 *  For security purposes, it should only be as large as needed.
 */
extern size_t SHTP_PACKAGE_MAXSIZE;

/*
 *  Defines the maximum depth the parser will attempt to parse a struct nest before it returns an error.
 *  For security purposes, when parsing a new package, this should only be as deep as needed.
 */
extern size_t SHTP_MAXDEPTH;

typedef int socket_t;

typedef enum : char {
  STRUCT = 'T',
  S_LIMITER = '(',
  S_DELIMITER = ')',
  A_DELIMITER = '[',
  A_LIMITER = ']',
  STRING = 'S',
  CHAR = 'C',
  UINT64 = 'U',
  UINT32 = 'u',
  INT64 = 'I',
  INT32 = 'i',
  RATIO = 'R',
  EOTS = '\0'
} Type;

typedef enum : int {
  ALL_GOOD = -1,
  RECEIVER_HASH_CORRUPTED = -100,
  PARSER_INVALID_ELEMENT = -200,
} Error;

typedef void* Package;

/* This struct holds all relevant information to a package type. */
typedef struct {
  char* package_type_name;
  Type* type_descriptor;
} PackageType;

/* Prints Error and returns errcode */
int error(Error errcode);

/* Returns ptr that is offset by offset from ptr */
void* ptr_offset(void* ptr, size_t offset);

/* Returns theoretical size of a field described by Type t */
size_t sizeof_type(Type t);

/* Returns whether or not a specific Type t would be correctly aligned at ptr */
bool is_aligned(Type t, void* ptr);

/* Returns how many Type elements are in the descriptor. Includes NULL termination. */
size_t descriptorlen(Type* type_descriptor);

/* Returns the theoretical length of a struct or field described by type_descriptor */
size_t calc_length(Type* type_descriptor);

/* Returns struct holding information for a package type */
PackageType create_package_type(char* package_type_name, char* type_descriptor);

/*
 *  Store package from the data stored at src_ptr into dest_ptr by using type_descriptor as a descriptor of what data lies there
 *  package_type_name refers to the string that singularly describes this typename.
 *  Handle these typenames like you would librarly macros: PREFIXES AND FULL CAPS ARE ENCOURAGED!
 *  Also returns a status code once done.
 */
Error create_package(PackageType package_type, void* src_ptr, Package* dest_ptr);

/*  Writes package onto a socket file descriptor
 *  Also returns a status code once done.
 */
Error send_package(socket_t socket_fd, Package package);

/* Sets PACKAGE_MAXSIZE to maxsize */
void set_maxsize(size_t maxsize);

/*  Retrieves package and stores it into a buffer.
 *  Note that this buffer will be allocated using malloc and must be freed. Consider specifying PACKAGE_MAXSIZE to determine what the maximum size of a package can be.
 *  Also returns a status code once done.
 */
int retrieve_package(char* package_type_name, socket_t socket_fd, void* buffer);