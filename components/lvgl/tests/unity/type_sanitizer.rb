module TypeSanitizer
  def self.sanitize_c_identifier(unsanitized)
    # convert filename to valid C identifier by replacing invalid chars with '_'
    unsanitized.gsub(/[-\/\\\.\,\s]/, '_')
  end
end
