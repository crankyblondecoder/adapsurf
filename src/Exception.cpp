#ifndef ADS_EXCEPTION_H
#define ADS_EXCEPTION_H

namespace adapsurf {

	/**
	 * Create and invoke thread entry point.
	 */
	class Exception
	{
		public:

			enum Error
			{
				UNKNOWN
			};

			virtual ~Exception(){}

			Exception(Error error) : _error{error} {}

		private:

			Error _error;
	};
}
#endif