#include "h.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{

using namespace std::string_view_literals;

constexpr std::string_view all_cards = "23456789TJQKA";

enum class CardScore : unsigned
{
};

CardScore to_score(char ch)
{
	return static_cast<CardScore>(all_cards.find_first_of(ch) + 1);
}

std::size_t to_index(CardScore score)
{
	return static_cast<std::size_t>(score) - 1;
}

enum class SetType : std::uint8_t
{
	High_card = 1,
	One_pair = 2,
	Two_pair = 3,
	Three_of_a_kind = 4,
	Full_house = 5,
	Four_of_a_kind = 6,
	Five_of_a_kind = 7,
};

class Set
{
public:
	Set(const char card_codes[5])
		: cards_{
			to_score(card_codes[0]),
			to_score(card_codes[1]),
			to_score(card_codes[2]),
			to_score(card_codes[3]),
			to_score(card_codes[4])}
	{
	}

	SetType set_type() const
	{
		unsigned cards[13] = {};
		for (std::size_t i = 0; i < 5; ++i)
		{
			cards[to_index(cards_[i])]++;
		}

		bool has_three = 0;
		std::uint8_t pairs = 0;
		for (const auto & card : cards)
		{
			if (card == 5)
			{
				return SetType::Five_of_a_kind;
			}
			if (card == 4)
			{
				return SetType::Four_of_a_kind;
			}
			if (card == 3)
			{
				has_three = true;
			}
			if (card == 2)
			{
				pairs++;
			}
		}
		if (has_three)
		{
			if (pairs == 1)
			{
				return SetType::Full_house;
			}
			return SetType::Three_of_a_kind;
		}
		if (pairs == 2)
		{
			return SetType::Two_pair;
		}
		if (pairs == 1)
		{
			return SetType::One_pair;
		}
		return SetType::High_card;
	}

	friend std::ostream & operator<<(std::ostream & out, const Set & set);

	auto operator<=>(const Set & other) const = default;

private:
	CardScore cards_[5];
};

std::ostream & operator<<(std::ostream & out, const Set & set)
{
	for (CardScore card : set.cards_)
	{
		out << all_cards[static_cast<std::size_t>(card) - 1];
	}
	return out;
}

struct SetWithBid
{
	Set set;
	unsigned bid;
};

SetWithBid read_line(std::istream & input)
{
	char set[6];
	unsigned bid;
	input >> set >> bid;
	return {Set(set), bid};
}

} // namespace

int task_7_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::vector<SetWithBid> sets;
	while (!input.bad() && !input.eof())
	{
		sets.push_back(read_line(input));
	}
	std::sort(
		sets.begin(),
		sets.end(),
		[](const SetWithBid & lhs, const SetWithBid & rhs)
		{
			if (lhs.set.set_type() == rhs.set.set_type())
			{
				return lhs.set < rhs.set;
			}
			return lhs.set.set_type() < rhs.set.set_type();
		});

	unsigned result = 0;
	for (std::size_t i = 0; i < sets.size(); ++i)
	{
		std::cout << sets[i].set << " (" << i + 1 << ") " << sets[i].bid << '\n';
		result += (i + 1) * sets[i].bid;
	}
	std::cout << result << '\n';

	return 0;
}
