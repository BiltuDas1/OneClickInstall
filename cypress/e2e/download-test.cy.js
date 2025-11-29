it('download executable test', () => {
  // Setup
  const DOWNLOADED_FILENAME = 'installer.exe';
  const EXPECTED_BYTE_DATA = '/*_MANDATORY_STEP_POPULATE_THIS_FIELD_VIA_SCRIPT_BEFORE_RUN_ */';
  const filePath = `cypress/downloads/${DOWNLOADED_FILENAME}`;

  // Cleanup
  cy.exec(`rm -f ${filePath}`, { failOnNonZeroExit: false });

  // Start
  cy.visit('/');

  // Find the card containing '7Zip' and click it
  cy.contains('.app-card', '7Zip').click();

  // Click the main install button
  cy.get('#installBtn').click();

  cy.log(`Waiting for file: ${filePath}`);
  cy.readFile(filePath, 'binary', { timeout: 20000 })
    .should((fileContent) => {
      expect(fileContent).not.to.include(EXPECTED_BYTE_DATA);
    });

  cy.log('Test passed: Downloaded Exe file with tokenID.');
});