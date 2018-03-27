<?php

/**
 *  Tests for MediaCommons Apache configuration
 *
 *  Tests for MediaCommons Apache configuration:
 *  https://jira.nyu.edu/jira/browse/MC-392
 *
 *  To run:
 *      # Install composer: https://getcomposer.org/doc/00-intro.md#installation-linux-unix-osx
 *      composer install
 *      vendor/phpunit/phpunit/phpunit tests/ApacheConfigurationTest.php
 *
 *  PHP version 5
 */

use PHPUnit\Framework\TestCase;

/**
 *  Contains tests for Apache configuration.
 *
 *  For full explanation of the redirection scheme, see:
 *  https://jira.nyu.edu/jira/browse/MC-392
 */
final class ApacheConfigurationTest extends TestCase {
    /**
     * All redirects should end up at these domains, at scheme CANONICAL_PROTOCOL.
     */
    const CANONICAL_HOSTS = [
        'dev' => 'dev.mediacommons.org',
        'stage' => 'stage.mediacommons.org',
        'prod' => 'mediacommons.org',
    ];
    const CANONICAL_PROTOCOL = 'http';

    /**
     * Which protocols to test.  Right now we are only testing http
     */
    const PROTOCOLS_TO_TEST = [ 'http' ];

    /**
     * Hosts to test, arranged by instance.
     */
    const HOSTS_TO_TEST = [
        'dev' => [
            'devmc2.dlib.nyu.edu',

            'dev.mediacommons.org',
            'dev.media-commons.org',

            'www-dev.mediacommons.org',
            'www-dev.media-commons.org',
        ],
        'stage' => [
            'stagemc2.dlib.nyu.edu',

            'stage.mediacommons.org',
            'stage.media-commons.org',

            'www-stage.mediacommons.org',
            'www-stage.media-commons.org',
        ],
        'prod' => [
            'mc2.dlib.nyu.edu',

            'mediacommons.org',
            'media-commons.org',

            'www.mediacommons.org',
            'www.media-commons.org',
        ]
    ];

    /**
     * Possible start paths
     */
    const PATHS = [ '', 'alt-ac', 'fieldguide', 'imr', 'intransition', 'mediacommons', 'tne' ];

    private static $ch;

    public static function setUpBeforeClass() {
        self::$ch = curl_init();

        curl_setopt( self::$ch, CURLOPT_NOBODY, TRUE );
        curl_setopt( self::$ch, CURLOPT_FOLLOWLOCATION, TRUE );
        curl_setopt( self::$ch, CURLOPT_RETURNTRANSFER, TRUE );
    }

    public static function tearDownAfterClass() {
        curl_close( self::$ch );
    }

    /**
     * @dataProvider generateTestUrls
     */
    public function testRedirect( $testUrl, $expectedEndUrl ) {
        list( $gotEndUrl, $responseContent, $error ) = $this->checkRedirect( $testUrl );

        if ( $error ) {
            $this->fail( "${testUrl} failed with error: \"${error}\"" );
        } else {
            $failureMessage = "${testUrl} redirected to incorrect URL ${gotEndUrl}; " .
                              "should redirect to ${expectedEndUrl}";
        }

        $this->assertEquals( $expectedEndUrl, $gotEndUrl, $failureMessage );
    }


    /**
     * Data provider for testRedirect test.
     *
     * Returns an array of arrays:
     * [
     *     [ "www-dev.mediacommons.org", "http://dev.mediacommons.org/",
     *     [ "www-dev.mediacommons.org/", "http://dev.mediacommons.org/",
     *     [ "www-dev.mediacommons.org/alt-ac", "http://dev.mediacommons.org/alt-ac/",
     *     [ "www-dev.mediacommons.org/alt-ac/", "http://dev.mediacommons.org/alt-ac/",
     *
     *     ...
     * ]
     *
     * The keys are start URLs, the values are the effective URLs that the user
     * is expected to be directed to.
     *
     * @return array test data structured according to the PHPUnit data provider spec
     */
    public function generateTestUrls() {
        $testUrls = [];

        foreach ( self::PROTOCOLS_TO_TEST as $protocol ) {

            foreach ( self::HOSTS_TO_TEST as $instance => $hostsToTest ) {

                foreach ( $hostsToTest as $hostToTest ) {

                    foreach ( self::PATHS as $path ) {

                        $canonicalHost = self::CANONICAL_HOSTS[ $instance ];

                        if ( $path ) {

                            if ( $path !== 'mediacommons' ) {
                                $canonicalUrl = self::CANONICAL_PROTOCOL . "://${canonicalHost}/${path}/";
                            } else {
                                // Umbrella site is served from root
                                $canonicalUrl = self::CANONICAL_PROTOCOL . "://${canonicalHost}/";
                            }

                            array_push( $testUrls, [ "${protocol}://${hostToTest}/${path}", $canonicalUrl ] );
                            array_push( $testUrls, [ "${protocol}://${hostToTest}/${path}/", $canonicalUrl ] );
                        } else {
                            $canonicalUrl = self::CANONICAL_PROTOCOL . "://${canonicalHost}/";

                            array_push( $testUrls, [ "${protocol}://${hostToTest}", $canonicalUrl ] );
                            array_push( $testUrls, [ "${protocol}://${hostToTest}/", $canonicalUrl ] );
                        }

                    }

                }

            }

        }

//        Uncomment this to update testGenerateTestUrls()
//        foreach ( $testUrls as $testUrlPair ) {
//            print "                [ '${testUrlPair[ 0 ]}', '${testUrlPair[ 1 ]}' ],\n";
//        }

        return $testUrls;
    }

    /**
     * Do curl request to get effective URL, page content, and error, if any.
     *
     * @param $url
     *
     * @return array the final URL in the redirect chain, the content on that
     *               page, and error, if any
     */
    public function checkRedirect( $url ) {
        curl_setopt( self::$ch, CURLOPT_URL, $url );

        $result = curl_exec( self::$ch );

        $effectiveUrl = curl_getinfo( self::$ch, CURLINFO_EFFECTIVE_URL );

        // Sometimes the URL starts with "HTTP:".  Normalize to lowercase.
        $effectiveUrl = preg_replace( '/^HTTP/', 'http', $effectiveUrl );
        // We don't use https yet, but might in the future.
        $effectiveUrl = preg_replace( '/^HTTPS/', 'https', $effectiveUrl );

        return [ $effectiveUrl, $result, curl_error( self::$ch ) ];
    }


    /* Tests for $this->generateTestUrls().  They serve mainly as a double-check
       and to aid refactoring and updating of the code in this test class. */

    /**
     * @dataProvider generateTestUrls
     *
     * Simple verification that testGenerateTestUrls is specifying correct canonical
     * URLs for dev, stage, and prod URLs.
     */
    public function testGenerateTestUrlsSimple( $testUrl, $expectedEndUrl ) {
        $urlParts = parse_url( $testUrl );
        $host = $urlParts[ 'host' ];
        $path = array_key_exists( 'path', $urlParts ) ? rtrim( $urlParts[ 'path' ], '/' ): null;

        if ( preg_match( '/dev/', $host ) ) {
            $correctExpectedEndUrl = self::CANONICAL_PROTOCOL . '://' . self::CANONICAL_HOSTS[ 'dev' ];
        } else if ( preg_match( '/stage/', $host ) ) {
            $correctExpectedEndUrl = self::CANONICAL_PROTOCOL . '://' . self::CANONICAL_HOSTS[ 'stage' ];
        } else {
            $correctExpectedEndUrl = self::CANONICAL_PROTOCOL . '://' . self::CANONICAL_HOSTS[ 'prod' ];
        }

        if ( $path && $path !== '/' && ! preg_match( '/\/mediacommons\/?$/', $path ) ) {
            $correctExpectedEndUrl .= "${path}/";
        } else {
            $correctExpectedEndUrl .= '/';
        }

        $this->assertEquals( $correctExpectedEndUrl, $expectedEndUrl );
    }

    /**
     * This is a comprehensive test of the correctness of $this->generateTestUrls().
     * The array was generated by temporary code in $this->generateTestUrls() and
     * then eyeball-checked.  It is used here to ensure correctness of
     * $this->generateTestUrls() after any code refactoring or updating of the logic.
     *
     * Why not just use this array or a table-driven approach in general?
     * We might be adding more test urls later, for example tests for https
     * redirections to http.  This can easily done in $this->generateTestUrls().
     * Editing a table by hand is more error-prone and tedious.
     * Depending on frequency and type of future code changes (if any), it may or
     * may not be worth keeping this test up-to-date.  testGenerateTestUrlsSimple()
     * is a very easy to maintain test, and can continue to provide a basic
     * double-check even if this test is dropped.
     */
    public function testGenerateTestUrls() {
        $expectedTestUrls =
            [
                [ 'http://devmc2.dlib.nyu.edu', 'http://dev.mediacommons.org/' ],
                [ 'http://devmc2.dlib.nyu.edu/', 'http://dev.mediacommons.org/' ],
                [ 'http://devmc2.dlib.nyu.edu/alt-ac', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://devmc2.dlib.nyu.edu/alt-ac/', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://devmc2.dlib.nyu.edu/fieldguide', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://devmc2.dlib.nyu.edu/fieldguide/', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://devmc2.dlib.nyu.edu/imr', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://devmc2.dlib.nyu.edu/imr/', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://devmc2.dlib.nyu.edu/intransition', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://devmc2.dlib.nyu.edu/intransition/', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://devmc2.dlib.nyu.edu/mediacommons', 'http://dev.mediacommons.org/' ],
                [ 'http://devmc2.dlib.nyu.edu/mediacommons/', 'http://dev.mediacommons.org/' ],
                [ 'http://devmc2.dlib.nyu.edu/tne', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://devmc2.dlib.nyu.edu/tne/', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://dev.mediacommons.org', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.mediacommons.org/', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.mediacommons.org/alt-ac', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://dev.mediacommons.org/alt-ac/', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://dev.mediacommons.org/fieldguide', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://dev.mediacommons.org/fieldguide/', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://dev.mediacommons.org/imr', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://dev.mediacommons.org/imr/', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://dev.mediacommons.org/intransition', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://dev.mediacommons.org/intransition/', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://dev.mediacommons.org/mediacommons', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.mediacommons.org/mediacommons/', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.mediacommons.org/tne', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://dev.mediacommons.org/tne/', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://dev.media-commons.org', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.media-commons.org/', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.media-commons.org/alt-ac', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://dev.media-commons.org/alt-ac/', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://dev.media-commons.org/fieldguide', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://dev.media-commons.org/fieldguide/', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://dev.media-commons.org/imr', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://dev.media-commons.org/imr/', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://dev.media-commons.org/intransition', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://dev.media-commons.org/intransition/', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://dev.media-commons.org/mediacommons', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.media-commons.org/mediacommons/', 'http://dev.mediacommons.org/' ],
                [ 'http://dev.media-commons.org/tne', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://dev.media-commons.org/tne/', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://www-dev.mediacommons.org', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.mediacommons.org/', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.mediacommons.org/alt-ac', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://www-dev.mediacommons.org/alt-ac/', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://www-dev.mediacommons.org/fieldguide', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://www-dev.mediacommons.org/fieldguide/', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://www-dev.mediacommons.org/imr', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://www-dev.mediacommons.org/imr/', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://www-dev.mediacommons.org/intransition', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://www-dev.mediacommons.org/intransition/', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://www-dev.mediacommons.org/mediacommons', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.mediacommons.org/mediacommons/', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.mediacommons.org/tne', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://www-dev.mediacommons.org/tne/', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://www-dev.media-commons.org', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.media-commons.org/', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.media-commons.org/alt-ac', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://www-dev.media-commons.org/alt-ac/', 'http://dev.mediacommons.org/alt-ac/' ],
                [ 'http://www-dev.media-commons.org/fieldguide', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://www-dev.media-commons.org/fieldguide/', 'http://dev.mediacommons.org/fieldguide/' ],
                [ 'http://www-dev.media-commons.org/imr', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://www-dev.media-commons.org/imr/', 'http://dev.mediacommons.org/imr/' ],
                [ 'http://www-dev.media-commons.org/intransition', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://www-dev.media-commons.org/intransition/', 'http://dev.mediacommons.org/intransition/' ],
                [ 'http://www-dev.media-commons.org/mediacommons', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.media-commons.org/mediacommons/', 'http://dev.mediacommons.org/' ],
                [ 'http://www-dev.media-commons.org/tne', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://www-dev.media-commons.org/tne/', 'http://dev.mediacommons.org/tne/' ],
                [ 'http://stagemc2.dlib.nyu.edu', 'http://stage.mediacommons.org/' ],
                [ 'http://stagemc2.dlib.nyu.edu/', 'http://stage.mediacommons.org/' ],
                [ 'http://stagemc2.dlib.nyu.edu/alt-ac', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://stagemc2.dlib.nyu.edu/alt-ac/', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://stagemc2.dlib.nyu.edu/fieldguide', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://stagemc2.dlib.nyu.edu/fieldguide/', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://stagemc2.dlib.nyu.edu/imr', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://stagemc2.dlib.nyu.edu/imr/', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://stagemc2.dlib.nyu.edu/intransition', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://stagemc2.dlib.nyu.edu/intransition/', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://stagemc2.dlib.nyu.edu/mediacommons', 'http://stage.mediacommons.org/' ],
                [ 'http://stagemc2.dlib.nyu.edu/mediacommons/', 'http://stage.mediacommons.org/' ],
                [ 'http://stagemc2.dlib.nyu.edu/tne', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://stagemc2.dlib.nyu.edu/tne/', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://stage.mediacommons.org', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.mediacommons.org/', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.mediacommons.org/alt-ac', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://stage.mediacommons.org/alt-ac/', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://stage.mediacommons.org/fieldguide', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://stage.mediacommons.org/fieldguide/', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://stage.mediacommons.org/imr', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://stage.mediacommons.org/imr/', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://stage.mediacommons.org/intransition', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://stage.mediacommons.org/intransition/', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://stage.mediacommons.org/mediacommons', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.mediacommons.org/mediacommons/', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.mediacommons.org/tne', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://stage.mediacommons.org/tne/', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://stage.media-commons.org', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.media-commons.org/', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.media-commons.org/alt-ac', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://stage.media-commons.org/alt-ac/', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://stage.media-commons.org/fieldguide', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://stage.media-commons.org/fieldguide/', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://stage.media-commons.org/imr', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://stage.media-commons.org/imr/', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://stage.media-commons.org/intransition', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://stage.media-commons.org/intransition/', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://stage.media-commons.org/mediacommons', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.media-commons.org/mediacommons/', 'http://stage.mediacommons.org/' ],
                [ 'http://stage.media-commons.org/tne', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://stage.media-commons.org/tne/', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://www-stage.mediacommons.org', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.mediacommons.org/', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.mediacommons.org/alt-ac', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://www-stage.mediacommons.org/alt-ac/', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://www-stage.mediacommons.org/fieldguide', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://www-stage.mediacommons.org/fieldguide/', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://www-stage.mediacommons.org/imr', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://www-stage.mediacommons.org/imr/', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://www-stage.mediacommons.org/intransition', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://www-stage.mediacommons.org/intransition/', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://www-stage.mediacommons.org/mediacommons', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.mediacommons.org/mediacommons/', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.mediacommons.org/tne', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://www-stage.mediacommons.org/tne/', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://www-stage.media-commons.org', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.media-commons.org/', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.media-commons.org/alt-ac', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://www-stage.media-commons.org/alt-ac/', 'http://stage.mediacommons.org/alt-ac/' ],
                [ 'http://www-stage.media-commons.org/fieldguide', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://www-stage.media-commons.org/fieldguide/', 'http://stage.mediacommons.org/fieldguide/' ],
                [ 'http://www-stage.media-commons.org/imr', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://www-stage.media-commons.org/imr/', 'http://stage.mediacommons.org/imr/' ],
                [ 'http://www-stage.media-commons.org/intransition', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://www-stage.media-commons.org/intransition/', 'http://stage.mediacommons.org/intransition/' ],
                [ 'http://www-stage.media-commons.org/mediacommons', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.media-commons.org/mediacommons/', 'http://stage.mediacommons.org/' ],
                [ 'http://www-stage.media-commons.org/tne', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://www-stage.media-commons.org/tne/', 'http://stage.mediacommons.org/tne/' ],
                [ 'http://mc2.dlib.nyu.edu', 'http://mediacommons.org/' ],
                [ 'http://mc2.dlib.nyu.edu/', 'http://mediacommons.org/' ],
                [ 'http://mc2.dlib.nyu.edu/alt-ac', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://mc2.dlib.nyu.edu/alt-ac/', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://mc2.dlib.nyu.edu/fieldguide', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://mc2.dlib.nyu.edu/fieldguide/', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://mc2.dlib.nyu.edu/imr', 'http://mediacommons.org/imr/' ],
                [ 'http://mc2.dlib.nyu.edu/imr/', 'http://mediacommons.org/imr/' ],
                [ 'http://mc2.dlib.nyu.edu/intransition', 'http://mediacommons.org/intransition/' ],
                [ 'http://mc2.dlib.nyu.edu/intransition/', 'http://mediacommons.org/intransition/' ],
                [ 'http://mc2.dlib.nyu.edu/mediacommons', 'http://mediacommons.org/' ],
                [ 'http://mc2.dlib.nyu.edu/mediacommons/', 'http://mediacommons.org/' ],
                [ 'http://mc2.dlib.nyu.edu/tne', 'http://mediacommons.org/tne/' ],
                [ 'http://mc2.dlib.nyu.edu/tne/', 'http://mediacommons.org/tne/' ],
                [ 'http://mediacommons.org', 'http://mediacommons.org/' ],
                [ 'http://mediacommons.org/', 'http://mediacommons.org/' ],
                [ 'http://mediacommons.org/alt-ac', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://mediacommons.org/alt-ac/', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://mediacommons.org/fieldguide', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://mediacommons.org/fieldguide/', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://mediacommons.org/imr', 'http://mediacommons.org/imr/' ],
                [ 'http://mediacommons.org/imr/', 'http://mediacommons.org/imr/' ],
                [ 'http://mediacommons.org/intransition', 'http://mediacommons.org/intransition/' ],
                [ 'http://mediacommons.org/intransition/', 'http://mediacommons.org/intransition/' ],
                [ 'http://mediacommons.org/mediacommons', 'http://mediacommons.org/' ],
                [ 'http://mediacommons.org/mediacommons/', 'http://mediacommons.org/' ],
                [ 'http://mediacommons.org/tne', 'http://mediacommons.org/tne/' ],
                [ 'http://mediacommons.org/tne/', 'http://mediacommons.org/tne/' ],
                [ 'http://media-commons.org', 'http://mediacommons.org/' ],
                [ 'http://media-commons.org/', 'http://mediacommons.org/' ],
                [ 'http://media-commons.org/alt-ac', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://media-commons.org/alt-ac/', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://media-commons.org/fieldguide', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://media-commons.org/fieldguide/', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://media-commons.org/imr', 'http://mediacommons.org/imr/' ],
                [ 'http://media-commons.org/imr/', 'http://mediacommons.org/imr/' ],
                [ 'http://media-commons.org/intransition', 'http://mediacommons.org/intransition/' ],
                [ 'http://media-commons.org/intransition/', 'http://mediacommons.org/intransition/' ],
                [ 'http://media-commons.org/mediacommons', 'http://mediacommons.org/' ],
                [ 'http://media-commons.org/mediacommons/', 'http://mediacommons.org/' ],
                [ 'http://media-commons.org/tne', 'http://mediacommons.org/tne/' ],
                [ 'http://media-commons.org/tne/', 'http://mediacommons.org/tne/' ],
                [ 'http://www.mediacommons.org', 'http://mediacommons.org/' ],
                [ 'http://www.mediacommons.org/', 'http://mediacommons.org/' ],
                [ 'http://www.mediacommons.org/alt-ac', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://www.mediacommons.org/alt-ac/', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://www.mediacommons.org/fieldguide', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://www.mediacommons.org/fieldguide/', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://www.mediacommons.org/imr', 'http://mediacommons.org/imr/' ],
                [ 'http://www.mediacommons.org/imr/', 'http://mediacommons.org/imr/' ],
                [ 'http://www.mediacommons.org/intransition', 'http://mediacommons.org/intransition/' ],
                [ 'http://www.mediacommons.org/intransition/', 'http://mediacommons.org/intransition/' ],
                [ 'http://www.mediacommons.org/mediacommons', 'http://mediacommons.org/' ],
                [ 'http://www.mediacommons.org/mediacommons/', 'http://mediacommons.org/' ],
                [ 'http://www.mediacommons.org/tne', 'http://mediacommons.org/tne/' ],
                [ 'http://www.mediacommons.org/tne/', 'http://mediacommons.org/tne/' ],
                [ 'http://www.media-commons.org', 'http://mediacommons.org/' ],
                [ 'http://www.media-commons.org/', 'http://mediacommons.org/' ],
                [ 'http://www.media-commons.org/alt-ac', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://www.media-commons.org/alt-ac/', 'http://mediacommons.org/alt-ac/' ],
                [ 'http://www.media-commons.org/fieldguide', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://www.media-commons.org/fieldguide/', 'http://mediacommons.org/fieldguide/' ],
                [ 'http://www.media-commons.org/imr', 'http://mediacommons.org/imr/' ],
                [ 'http://www.media-commons.org/imr/', 'http://mediacommons.org/imr/' ],
                [ 'http://www.media-commons.org/intransition', 'http://mediacommons.org/intransition/' ],
                [ 'http://www.media-commons.org/intransition/', 'http://mediacommons.org/intransition/' ],
                [ 'http://www.media-commons.org/mediacommons', 'http://mediacommons.org/' ],
                [ 'http://www.media-commons.org/mediacommons/', 'http://mediacommons.org/' ],
                [ 'http://www.media-commons.org/tne', 'http://mediacommons.org/tne/' ],
                [ 'http://www.media-commons.org/tne/', 'http://mediacommons.org/tne/' ],
            ];

        $testUrls = $this->generateTestUrls();

        $this->assertEquals( $expectedTestUrls, $testUrls,
                             'testGenerateUrls() did not create the correct set of test URLS' );
    }

}
