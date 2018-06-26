#include "TextPlain.h"

namespace DataVariant
{
	TextPlain::TextPlain() noexcept
	{
		this->data_variant_name = "text/plain";
	}

	virtual bool DataVariant :: parse(
		const std::string &buf,
		Transfer::request_data *rd,
		DataReceiver *dr
	)const
	{
					if (buf.empty() ) {
			return 0 == dr->full_size || dr->full_size != dr->recv_total;
		}

		for (
			size_t var_pos = 0, var_end = 0;
			std::string::npos != var_end;
			var_pos = var_end + 1
		) {
			//Search for Next Variable
			var_end = buf.find('&', var_pos);

			if (std::string::npos == var_end) {   		//No & any more
				if (dr->full_size != dr->recv_total) {	//Not all data received
					dr->left = buf.size() - var_pos;	//Minus Data receive
					return true;
				}
			}

			//Search for Variable
			size_t delimiter = buf.find('=', var_pos);

			
			if(delimiter < var_end)
			{
				//Get Name
				std::string var_name = buf.substr(var_pos, delimiter - var_pos);

				++delimiter;

				//Get Value
				std::string var_value = buf.substr(
					delimiter,
					std::string::npos != var_end
						? var_end - delimiter
						: std::string::npos
				);

				//Store Name & Value
				rd->incoming_data.emplace(
					std::move(var_name),
					std::move(var_value)
				);
			}
			else
			{
				//Get Name
				std::string var_name = buf.substr(
					var_pos,
					std::string::npos != var_end
						? var_end - var_pos
						: std::string::npos
				);

				//Store Name & Null Value
				rd->incoming_data.emplace(
					std::move(var_name),
					std::string()
				);
			} 
		}

		return true;
	}


}
