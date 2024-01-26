#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <cmath>

#define TOP_PRIMES_COUNT	10
#define THREAD_COUNT		8
#define PRIME_RANGE			100000000

typedef int prime_t;

struct prime_data
{
	std::mutex *mtx;

	// high to low
	prime_t top_primes[ TOP_PRIMES_COUNT ];
	size_t ntop_primes;

	prime_t primes_total;
	int64_t primes_sum;
};

static int is_prime( prime_t n )
{
	// simple cases
	if ( n == 2 || n == 3 )
		return 1;

	if ( n <= 1 || n % 2 == 0 || n % 3 == 0 )
		return 0;

	// check factors
	prime_t n_sqrt = std::sqrt( n );
	for ( prime_t i = 5; i <= n_sqrt; i += 6 )
		if ( n % i == 0 || n % ( i + 2 ) == 0 )
			return 0;

	return 1;
}

static void insert_tp( struct prime_data *data, prime_t p )
{
	// smaller than all values
	if ( data->ntop_primes >= TOP_PRIMES_COUNT && data->top_primes[ TOP_PRIMES_COUNT - 1 ] > p )
		return;

	// fill array if it is not filled
	if ( data->ntop_primes < TOP_PRIMES_COUNT )
	{
		data->top_primes[ data->ntop_primes ] = p;
		data->ntop_primes++;
	}

	// index of current prime
	size_t i = data->ntop_primes - 1;

	if ( data->top_primes[ i ] > p )
		return;

	// replace current smallest prime
	data->top_primes[ i ] = p;

	// insertion sort
	while ( i > 0 && data->top_primes[ i ] > data->top_primes[ i - 1 ] )
	{
		prime_t tmp = data->top_primes[ i ];
		data->top_primes[ i ] = data->top_primes[ i - 1 ];
		data->top_primes[ i - 1 ] = tmp;
		i--;
	}
}

static void compute_primes_util( struct prime_data *data, std::atomic< prime_t > *ticket )
{
	prime_t current_ticket;
	while ( ( current_ticket = ticket->fetch_sub( 1 ) ) > 0 )
	{
		if ( is_prime( current_ticket ) )
		{
			data->mtx->lock();

			data->primes_total++;
			data->primes_sum += current_ticket;
			insert_tp( data, current_ticket );

			data->mtx->unlock();
		}
	}
}

static struct prime_data compute_primes( int r )
{
	std::mutex mtx;
	struct prime_data data = { 0 };
	data.mtx = &mtx;

	std::thread workers[ THREAD_COUNT ];
	std::atomic< prime_t > ticket = r;

	// init worker threads
	for ( size_t i = 0; i < THREAD_COUNT; i++ )
	{
		workers[ i ] = std::thread( compute_primes_util, &data, &ticket );
	}

	// wait for threads to finish
	for ( size_t i = 0; i < THREAD_COUNT; i++ )
	{
		workers[ i ].join();
	}

	return data;
}

static void write_prime_data( struct prime_data *data, double dur )
{
	std::ofstream f;
	f.open( "primes.txt" );

	f << std::setprecision( 2 ) << std::fixed;
	f << dur << " " << data->primes_total << " " << data->primes_sum << std::endl;

	f << data->top_primes[ TOP_PRIMES_COUNT - 1 ];
	for ( ssize_t i = data->ntop_primes - 2; i >= 0; i-- )
	{
		f << ", " << data->top_primes[ i ];
	}
	f << std::endl;

	f.close();
}

int main( void )
{
	auto start = std::chrono::high_resolution_clock::now();

	struct prime_data data = compute_primes( PRIME_RANGE );

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration< double > dur = end - start;

	write_prime_data( &data, dur.count() );

	return 0;
}
