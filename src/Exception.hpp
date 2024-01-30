#ifndef ADS_EXCEPTION_H
#define ADS_EXCEPTION_H

#include <string>

using namespace std;

namespace adapsurf {

	/**
	 * Create and invoke thread entry point.
	 */
	class Exception
	{
		public:

			enum Error
			{
				UNKNOWN,
				/** Could not open DRM device file. */
				DRM_BAD_DEV_FILE
			};

			virtual ~Exception(){}

			/**
			 * @param error Error code.
			 * @param description Description of error.
			 */
			Exception(Error error, string& description) : _error{error}, _descr{description} {}

		private:

			/** Error code. */
			Error _error;

			/** Description of error. */
			string _descr;
	};
}

#endif