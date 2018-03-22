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
     * Possible start URL parent domains
     */
    const PARENT_DOMAINS = [ 'mediacommons.org', 'media-commons.org' ];

    /**
     * Possible start paths
     */
    const PATHS = [ '', 'alt-ac', 'fieldguide', 'imr', 'intransition', 'mediacommons', 'tne' ];

    /** Possible start prefixes to be appended (with/withouth hyphen) to the
     * INSTANCE basename values
     *
     */
    const PREFIXES = [ 'www', '' ];

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

        foreach ( self::INSTANCES as $instance ) {
            $basename = $instance[ 'basename' ];

            foreach(self::PARENT_DOMAINS as $parentDomain ) {

                foreach( self::PREFIXES as $prefix ) {
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
                            $canonicalFullyQualifiedDomainName = $basename . '.' . self::CANONICAL_PARENT_DOMAIN_NAME;
                            $fullyQualifiedDomainName = "${subdomain}.${parentDomain}";
                        } else {
                            $canonicalFullyQualifiedDomainName = self::CANONICAL_PARENT_DOMAIN_NAME;
                            $fullyQualifiedDomainName = "${parentDomain}";
                        }

                        if ( $path ) {
                            $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/${path}/";

                            array_push( $testUrls, [ "${fullyQualifiedDomainName}/${path}", $canonicalUrl ] );
                            array_push( $testUrls, [ "${fullyQualifiedDomainName}/${path}/", $canonicalUrl ] );
                        } else {
                            $canonicalUrl = self::CANONICAL_PROTOCOL  . "://${canonicalFullyQualifiedDomainName}/";

                            array_push( $testUrls, [ "${fullyQualifiedDomainName}", $canonicalUrl ] );
                            array_push( $testUrls, [ "${fullyQualifiedDomainName}/", $canonicalUrl ] );
                        }

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
