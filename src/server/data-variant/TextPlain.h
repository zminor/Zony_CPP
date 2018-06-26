#ifndef __TEXTPLAIN_H__
#define __TEXTPLAIN_H__

namespace DataVariant
{
	class TextPlain :: public Abstract
	{
	public:
		TextPlain() noexcept;

	public:
		virtual bool parse(
			const std::string &buf, 
			Transfer::request_data *rd, 
			DataReceiver *dr
		) const;
	};

}
#endif
