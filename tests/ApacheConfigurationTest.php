<?php

/**
 *  Tests for MediaCommons Apache configuration
 *
 *  Tests for MediaCommons Apache configuration:
 *  https://jira.nyu.edu/jira/browse/MC-392
 *
 *  To run:
 *
 *      composer install
 *      vendor/phpunit/phpunit/phpunit tests/ApacheConfigurationTest.php
 *
 *  PHP version 5
 */

use PHPUnit\Framework\TestCase;

/**
 *  Contains tests for Apache configuration.  At the moment, only redirects are
 *  being tested.
 *
 *  For full explanation of the redirection scheme, see:
 *  https://jira.nyu.edu/jira/browse/MC-392
 */
final class ApacheConfigurationTest extends TestCase {
    /**
     * All redirects should end up at this protocol and domain or subdomain of this
     * domain.
     */
    const CANONICAL_PARENT_DOMAIN_NAME = 'mediacommons.org';
    const CANONICAL_PROTOCOL = 'http';

    /**
     * Which protocols to test.  Right now we are only testing http
     */
    const PROTOCOLS_TO_TEST = [ 'http' ];

    /**
     * The web server instances that we are testing.
     *
     * Basenames are used with PREFIX values to set subdomain.
     */
    const INSTANCES = [
        'dev' => [
            'basename' => 'dev',
        ],
        'stage' => [
            'basename' => 'stage',
        ],
        'prod' => [
            'basename' => '',
        ]
    ];

    /**
     * Possible start paths
     */
    const PATHS = [ '', 'alt-ac', 'fieldguide', 'imr', 'intransition', 'mediacommons', 'tne' ];

    /**
     * @dataProvider generateTestUrls
     */
    public function testRedirect( $testUrl, $expectedEndUrl ) {
        $gotEndUrl = $this->checkRedirect( $testUrl, $expectedEndUrl );

        $failureMessage = "${testUrl} redirected to incorrect URL ${expectedEndUrl}; " .
                          "should redirect to ${gotEndUrl}";

        $this->assertEquals( $expectedEndUrl, $gotEndUrl, $failureMessage );
    }

    /**
     * @dataProvider generateTestUrls
     *
     * Simple verification that testGenerateTestUrls is creating correct test data
     */
    public function testGenerateTestUrls( $testUrl, $expectedEndUrl ) {
        $urlParts = parse_url( $testUrl );
        $host = $urlParts[ 'host' ];
        $path = array_key_exists( 'path', $urlParts ) ? rtrim( $urlParts[ 'path' ], '/' ): null;

        if ( preg_match( '/dev/', $host ) ) {
            $correctExpectedEndUrl = self::CANONICAL_PROTOCOL . '://dev.' . self::CANONICAL_PARENT_DOMAIN_NAME;
        } else if ( preg_match( '/stage/', $host ) ) {
            $correctExpectedEndUrl = self::CANONICAL_PROTOCOL . '://stage.' . self::CANONICAL_PARENT_DOMAIN_NAME;
        } else {
            $correctExpectedEndUrl = self::CANONICAL_PROTOCOL . '://' . self::CANONICAL_PARENT_DOMAIN_NAME;
        }

        if ( $path && $path !== '/' ) {
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
        $testUrls = $this->generateTestUrlsMediacommonsDomain();

        $testUrls = array_merge( $testUrls, $this->generateTestUrlsDlibDomain() );

        return $testUrls;
    }

    public function generateTestUrlsMediacommonsDomain() {
        /**
         * Possible start URL parent domains
         */
        $parentDomains = [ 'mediacommons.org', 'media-commons.org' ];

        /** Possible start prefixes to be appended (with/withouth hyphen) to the
         * INSTANCE basename values
         *
         */
        $prefixes = [ 'www', '' ];

        $testUrls = [];

        foreach ( self::PROTOCOLS_TO_TEST as $protocol ) {

            foreach ( self::INSTANCES as $instance ) {
                $basename = $instance[ 'basename' ];

                foreach( $parentDomains as $parentDomain ) {

                    foreach( $prefixes as $prefix ) {
                        $subdomain = $basename;

                        if ( $prefix ) {
                            if ( $subdomain ) {
                                $subdomain = $prefix . '-' . $subdomain;
                            } else {
                                $subdomain = $prefix;
                            }
                        } else {
                            // Do nothing
                        }

                        foreach (self::PATHS as $path ) {

                            if ( $subdomain ) {
                                $canonicalFullyQualifiedDomainName = $basename ?
                                    $basename . '.' . self::CANONICAL_PARENT_DOMAIN_NAME :
                                    self::CANONICAL_PARENT_DOMAIN_NAME;
                                $fullyQualifiedDomainName = "${subdomain}.${parentDomain}";
                            } else {
                                $canonicalFullyQualifiedDomainName = self::CANONICAL_PARENT_DOMAIN_NAME;
                                $fullyQualifiedDomainName = "${parentDomain}";
                            }

                            if ( $path ) {
                                $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/${path}/";

                                array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}/${path}", $canonicalUrl ] );
                                array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}/${path}/", $canonicalUrl ] );
                            } else {
                                $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/";

                                array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}", $canonicalUrl ] );
                                array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}/", $canonicalUrl ] );
                            }

                        }

                    }

                }
            }

        }

        return $testUrls;
    }

    public function generateTestUrlsDlibDomain() {
        $domainSuffix = 'mc2.dlib.nyu.edu';

        $testUrls = [];

        foreach ( self::INSTANCES as $instance ) {
            $basename = $instance[ 'basename' ];

            $fullyQualifiedDomainName = "${basename}${domainSuffix}";

            if ( $basename ) {
                $canonicalFullyQualifiedDomainName = $basename . '.' . self::CANONICAL_PARENT_DOMAIN_NAME;
            } else {
                $canonicalFullyQualifiedDomainName = self::CANONICAL_PARENT_DOMAIN_NAME;
            }

            foreach ( self::PROTOCOLS_TO_TEST as $protocol ) {
                foreach ( self::PATHS as $path ) {
                    if ( $path ) {
                        $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/${path}/";

                        array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}/${path}", $canonicalUrl ] );
                        array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}/${path}/", $canonicalUrl ] );
                    } else {
                        $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/";

                        array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}", $canonicalUrl ] );
                        array_push( $testUrls, [ "${protocol}://${fullyQualifiedDomainName}/", $canonicalUrl ] );
                    }
                }
            }

        }

        return $testUrls;
    }

    public function checkRedirect( $url ) {
        $ch = curl_init();

        curl_setopt( $ch, CURLOPT_URL, $url );
        curl_setopt( $ch, CURLOPT_FOLLOWLOCATION, TRUE );
        curl_setopt( $ch, CURLOPT_NOBODY, TRUE );

        curl_exec( $ch );

        $effectiveUrl = curl_getinfo( $ch, CURLINFO_EFFECTIVE_URL );

        // Sometimes the URL starts with "HTTP:".  Normalize to lowercase.
        $effectiveUrl = preg_replace( '/^HTTP/', 'http', $effectiveUrl );
        // We don't use https yet, but might in the future.
        $effectiveUrl = preg_replace( '/^HTTPS/', 'https', $effectiveUrl );

        return $effectiveUrl;
    }

}
