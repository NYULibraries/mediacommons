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
        'stage' => 'dev.mediacommons.org',
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
     * @dataProvider generateTestUrls
     *
     * Simple verification that testGenerateTestUrls is specifying correct canonical
     * URLs for dev, stage, and prod URLs.
     */
    public function testGenerateTestUrls( $testUrl, $expectedEndUrl ) {
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

}
