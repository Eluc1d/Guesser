/**
 * Unit Tests for the class
**/

#include <gtest/gtest.h>
#include "Guesser.h"

class GuesserTest : public ::testing::Test
{
	protected:
		GuesserTest(){} //constructor runs before each test
		virtual ~GuesserTest(){} //destructor cleans up after tests
		virtual void SetUp(){} //sets up before each test (after constructor)
		virtual void TearDown(){} //clean up after each test, (before destructor)
};

// Constructor tests - covering normal, edge cases, and truncation

TEST(GuesserTest, constructor_normal_and_match)
{
	Guesser guesser("secret");
	ASSERT_TRUE(guesser.match("secret"));
}

TEST(GuesserTest, constructor_empty_secret)
{
	Guesser guesser("");
	ASSERT_TRUE(guesser.match(""));
}

TEST(GuesserTest, constructor_truncates_at_32_chars)
{
	string long_secret = "this_is_a_very_long_secret_phrase_that_exceeds_32_characters";
	Guesser guesser(long_secret);
	string first_32 = long_secret.substr(0, 32);
	ASSERT_EQ(0, guesser.distance(first_32));
}

// Distance function - comprehensive coverage in fewer tests

TEST(GuesserTest, distance_various_cases)
{
	Guesser guesser("hello");
	// Identical strings
	ASSERT_EQ(0, guesser.distance("hello"));
	// One character different
	ASSERT_EQ(1, guesser.distance("hallo"));
	// Completely different
	ASSERT_EQ(5, guesser.distance("zzzzz"));
	// Empty guess
	ASSERT_EQ(5, guesser.distance(""));
}

TEST(GuesserTest, distance_length_differences)
{
	Guesser guesser("test");
	// Guess shorter than secret
	ASSERT_EQ(2, guesser.distance("te"));
	// Guess longer than secret
	ASSERT_EQ(3, guesser.distance("testing"));
}

TEST(GuesserTest, distance_very_long_guess_capped)
{
	Guesser guesser("hi");
	// Very long guess, distance should cap at secret length (2)
	ASSERT_EQ(2, guesser.distance("verylongguess"));
}

// Match - basic functionality

TEST(GuesserTest, match_wrong_guess)
{
	Guesser guesser("password");
	ASSERT_FALSE(guesser.match("wrong"));
}

TEST(GuesserTest, match_case_sensitive)
{
	Guesser guesser("Secret");
	ASSERT_FALSE(guesser.match("secret"));
}

// Match - remaining guesses logic (consolidated)

TEST(GuesserTest, match_three_wrong_guesses_then_locked)
{
	Guesser guesser("secret");
	// Three wrong guesses within distance 2
	ASSERT_FALSE(guesser.match("secrat")); // distance 1, 2 remaining
	ASSERT_FALSE(guesser.match("secrit")); // distance 1, 1 remaining
	ASSERT_FALSE(guesser.match("secrot")); // distance 1, 0 remaining
	// Fourth guess fails even if correct - locked due to no remaining
	ASSERT_FALSE(guesser.match("secret"));
}

TEST(GuesserTest, match_correct_after_wrong_guesses)
{
	Guesser guesser("test");
	ASSERT_FALSE(guesser.match("tast")); // wrong
	ASSERT_FALSE(guesser.match("tost")); // wrong
	ASSERT_TRUE(guesser.match("test"));  // correct on 3rd try
}

// Match - brute force detection (critical boundary tests)

TEST(GuesserTest, match_brute_force_locks_immediately)
{
	Guesser guesser("secret");
	// Distance > 2 triggers immediate lock
	ASSERT_FALSE(guesser.match("xxxxxx")); // distance 6 > 2
	// Even correct guess now fails
	ASSERT_FALSE(guesser.match("secret"));
}

TEST(GuesserTest, match_distance_exactly_2_no_lock)
{
	Guesser guesser("abcdef");
	// Distance exactly 2 should NOT lock
	ASSERT_FALSE(guesser.match("XXcdef")); // distance 2
	ASSERT_TRUE(guesser.match("abcdef"));  // should still work
}

TEST(GuesserTest, match_distance_3_locks)
{
	Guesser guesser("abcdef");
	// Distance 3 > 2, should lock
	ASSERT_FALSE(guesser.match("XXXdef")); // distance 3
	ASSERT_FALSE(guesser.match("abcdef")); // locked
}

// Match - edge cases

TEST(GuesserTest, match_empty_guess_on_nonempty_secret)
{
	Guesser guesser("test");
	// Empty guess has large distance, should lock
	ASSERT_FALSE(guesser.match(""));
	ASSERT_FALSE(guesser.match("test")); // locked
}

TEST(GuesserTest, match_single_char_secret)
{
	Guesser guesser("x");
	ASSERT_FALSE(guesser.match("y"));
	ASSERT_TRUE(guesser.match("x"));
}

// Match - lock persistence

TEST(GuesserTest, match_locked_stays_locked)
{
	Guesser guesser("pass");
	ASSERT_FALSE(guesser.match("xxxx")); // lock via brute force
	// Multiple attempts should all fail
	ASSERT_FALSE(guesser.match("pass"));
	ASSERT_FALSE(guesser.match("pass"));
}

// Additional strategic tests for coverage

TEST(GuesserTest, match_special_chars_and_numbers)
{
	Guesser guesser("p@ss123");
	ASSERT_TRUE(guesser.match("p@ss123"));
}

TEST(GuesserTest, distance_and_match_integration)
{
	Guesser guesser("12345");
	// Test distance calculation with match behavior
	ASSERT_EQ(1, guesser.distance("X2345"));
	ASSERT_FALSE(guesser.match("X2345")); // distance 1, doesn't lock
	ASSERT_EQ(1, guesser.distance("1X345"));
	ASSERT_FALSE(guesser.match("1X345")); // distance 1, doesn't lock
	ASSERT_TRUE(guesser.match("12345"));  // correct on 3rd try
}
